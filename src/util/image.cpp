#include "image.h"

#include <stdio.h>
#include <setjmp.h>

extern "C" {
#include <jpeglib.h>
#include <png.h>
}

#define JPEG_QUALITY 90


Image::Image()
{
    width = 0;
    height = 0;
    numChannels = 0;
    data = NULL;
}

Image::~Image()
{
    SAFE_DELETE( data );
}

void Image::Create( u32 _width, u32 _height, u32 _numChannels, u8* _data )
{
    width = _width;
    height = _height;
    numChannels = _numChannels;

    if (data != NULL)
    {
        SAFE_DELETE( data );
    }

    data = new u8 [ width * height * numChannels ];
    memcpy( data, _data, width * height * numChannels );
}

bool Image::Read( const char* _path )
{
    const char* suffix = strrchr( _path, '.' );
    if( suffix != NULL )
    {
        if( strcmp( suffix, ".jpg" ) == 0 )
        {
            return ReadJPG( _path );
        }
        else if( strcmp( suffix, ".png" ) == 0 )
        {
            return ReadPNG( _path );
        }
        else 
        {
            kLogError( "The image type (%s) is not supported by Kulani.", suffix );
            return false;
        }
    }

    kLogError( "Unregonized image type. " );
    return false;
}

bool Image::Write( const char* _path )
{
    const char* suffix = strrchr( _path, '.' );
    if( suffix != NULL )
    {
        if( strcmp( suffix, ".jpg" ) == 0 )
        {
            return WriteJPG( _path );
        }
        else if( strcmp( suffix, ".png" ) == 0 )
        {
            return WritePNG( _path );
        }
        else 
        {
            kLogError( "The image type (%s) is not supported by Kulani.", suffix );
            return false;
        }
    }

    kLogError( "Unregonized image type. " );
    return false;
}

///////////////////////////////////////////////////////////////////////////////////// 
// JPEG read and write
///////////////////////////////////////////////////////////////////////////////////// 

// JPG loading and storing utils 
struct KIJPGDecodeStruct
{
    jpeg_source_mgr smgr;          ///< LibJPG source manager
    FILE*           inputFp;    ///< The input stream.
    JOCTET*         buffer;                ///< Buffer containing decoded data.
};

// Helper structure used for JPG compressing. 
struct KIJPGEncodeStruct
{
    jpeg_destination_mgr dmgr;      ///< LibJPG destination manager. 
    FILE*                outputFp;   ///< The output stream. 
    JOCTET*              buffer;                 ///< Buffer containing compressed data. 
};

// JPG error management struct; custom extension stores a setjmp context for returning from exception 
struct KIJPGErrorManager 
{
    jpeg_error_mgr pub;    ///< "public" fields 
    jmp_buf        setjmp_buffer; ///< for "try-catch" error handling in C.
};

// Maximum buffer size for JPG compress buffers. 
static const u32 KI_IMAGE_JPG_MAXIMUM_BUFFER_SIZE = 256;

// JPGlib decode callback function. 
static void ImageJPGDecodeInitFunction( j_decompress_ptr _cinfo )
{
    KIJPGDecodeStruct* decodeStruct = (KIJPGDecodeStruct*)_cinfo->src;

    decodeStruct->buffer = (JOCTET*)(*_cinfo->mem->alloc_small)((j_common_ptr)(void*)_cinfo, 
            JPOOL_IMAGE, KI_IMAGE_JPG_MAXIMUM_BUFFER_SIZE * sizeof(JOCTET));
    decodeStruct->smgr.bytes_in_buffer = 0;
}

// JPGlib call back function for filling input buffer while decoding. Lint error for constant parameter suppressed. 
static boolean ImageJPGDecodeFillInputBufferFunction( j_decompress_ptr _cinfo )
{
    u32 bytesRead;
    KIJPGDecodeStruct* decodeStruct = (KIJPGDecodeStruct*)_cinfo->src;

    bytesRead = fread((u8*)decodeStruct->buffer, KI_IMAGE_JPG_MAXIMUM_BUFFER_SIZE, 1, decodeStruct->inputFp);
    decodeStruct->smgr.next_input_byte = decodeStruct->buffer;
    decodeStruct->smgr.bytes_in_buffer = bytesRead;

    return true;
}

// Internal callback function for skipping input data while decoding JPG data. Suppress the usage of long and const. 
static void ImageJPGDecodeSkipInputData( j_decompress_ptr _cinfo, long _num_bytes )
{
    KIJPGDecodeStruct* decodeStruct = (KIJPGDecodeStruct*)_cinfo->src;

    if( _num_bytes > 0)
    {
        /* Skip buffer until done, avoiding that bytes in buffer never gets below zero. */
        while( _num_bytes > (long)decodeStruct->smgr.bytes_in_buffer )
        {
            _num_bytes -= (long)decodeStruct->smgr.bytes_in_buffer;
            ImageJPGDecodeFillInputBufferFunction( _cinfo ); /*lint !e534 Return value ignored. */
        }
    }
    decodeStruct->smgr.next_input_byte += (size_t)_num_bytes;
    decodeStruct->smgr.bytes_in_buffer -= (size_t)_num_bytes;
}

// JPGlib call back function for terminating source while decoding. 
static void ImageJPGDecodeTerminateSource(j_decompress_ptr cinfo)
{
}

// JPGlib callback function for initializing destination while compressing. 
static void ImageJPGEncodeInitDestination( j_compress_ptr _cinfo )
{
    KIJPGEncodeStruct* encodeStruct = (KIJPGEncodeStruct*)_cinfo->dest;

    encodeStruct->buffer = (JOCTET*)(*_cinfo->mem->alloc_small)((j_common_ptr)(void*)_cinfo, 
            JPOOL_IMAGE, KI_IMAGE_JPG_MAXIMUM_BUFFER_SIZE * sizeof(JOCTET));
    encodeStruct->dmgr.next_output_byte = (JOCTET*)encodeStruct->buffer;
    encodeStruct->dmgr.free_in_buffer = KI_IMAGE_JPG_MAXIMUM_BUFFER_SIZE;
}

// JPGlib callback function for emptying buffer while compressing. Constant function parameters suppressed from lint. 
static boolean ImageJPGEncodeEmptyBuffer( j_compress_ptr _cinfo )
{
    KIJPGEncodeStruct* encodeStruct = (KIJPGEncodeStruct*)_cinfo->dest;
    u32 bytesWritten = fwrite( (u8*)encodeStruct->buffer, KI_IMAGE_JPG_MAXIMUM_BUFFER_SIZE, 1, encodeStruct->outputFp );

    encodeStruct->dmgr.next_output_byte = encodeStruct->buffer;
    encodeStruct->dmgr.free_in_buffer = KI_IMAGE_JPG_MAXIMUM_BUFFER_SIZE;

    return TRUE;
}

// JPGlib callback function for terminating destination buffer while compressing. Constant function parameters suppressed from lint. 
static void ImageJPGEncodeTermDestination( j_compress_ptr _cinfo )
{
    KIJPGEncodeStruct* encodeStruct = (KIJPGEncodeStruct*)_cinfo->dest;
    u32 size = KI_IMAGE_JPG_MAXIMUM_BUFFER_SIZE - encodeStruct->dmgr.free_in_buffer;

    /* If there are remaining bytes to be written, write them. */
    if( size > 0 )
    {
        u32 bytesWritten = fwrite((u8*)encodeStruct->buffer, 
                size, 1, encodeStruct->outputFp);
        KI_ASSERT(bytesWritten == size, "OpenGL JPG doesn't read required bytes");
    }
}

// Replacement for the standard output_message method 
static void ImageJPGErrorMessage( j_common_ptr _cinfo )
{
    char buffer[JMSG_LENGTH_MAX];

    /* Show a clear-text error message */
    (*_cinfo->err->format_message)( _cinfo, buffer );  
    kLogError(buffer);
}

// Replacement for the standard error_exit method 
static void ImageJPGErrorHandler( j_common_ptr _cinfo )
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    KIJPGErrorManager *myerr = (KIJPGErrorManager*)_cinfo->err;

    // Display the message. 
    // TODO: instead of / in addition to logging, pass the message over the 
    //          longjmp so that it can be thrown with the kzsErrorThrow. 
    // (*cinfo->err->output_message) (cinfo); 
    ImageJPGErrorMessage( _cinfo );

    // Return control to the setjmp point.
    longjmp( myerr->setjmp_buffer, 1 );
}

bool Image::ReadJPG( const char* _path )
{
    FILE* fp = fopen( _path, "rb" );
    if( fp == NULL )
    {
        kLogError( "Failed to read %s.", _path );
        return false;
    }

    KIJPGErrorManager jerr;

    KIJPGDecodeStruct* decodeStruct = NULL;
    u8** rowPointers                = NULL;
    data                            = NULL;

    // We set up the normal JPG error routines, then override error_exit. 
    jpeg_decompress_struct cinfo;
    cinfo.err               = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit     = ImageJPGErrorHandler;
    jerr.pub.output_message = ImageJPGErrorMessage;

    // Establish the setjmp return context; "catch block" for "exceptions" during decompression.
    if( setjmp( jerr.setjmp_buffer ) )
    {
        // If we get here, the JPG code has signaled an error. 
        // Clean up the JPG object, close the input file, and relay the error forward. 
        // TODO: use error message from JPG 
        delete [] rowPointers;
        delete [] data;
        delete decodeStruct;
        jpeg_destroy_decompress( &cinfo );        
        kLogError( "Error while loading a JPG image" );
        return false;
    }

    // Create decompression info struct.
    jpeg_create_decompress( &cinfo );

    // Create and initialize the decode structure with function pointers and pass the input stream. */
    decodeStruct = new KIJPGDecodeStruct;

    decodeStruct->inputFp                  = fp;
    decodeStruct->smgr.init_source         = ImageJPGDecodeInitFunction;
    decodeStruct->smgr.fill_input_buffer   = ImageJPGDecodeFillInputBufferFunction;
    decodeStruct->smgr.skip_input_data     = ImageJPGDecodeSkipInputData;
    decodeStruct->smgr.resync_to_restart   = jpeg_resync_to_restart;
    decodeStruct->smgr.term_source         = ImageJPGDecodeTerminateSource;

    // Assign our decode struct for source manager of decompress info. 
    cinfo.src = (jpeg_source_mgr*)decodeStruct;

    // Read the header, may "throw exceptions" 
    u32 jpegError = (u32)jpeg_read_header( &cinfo, TRUE );
    if( jpegError != JPEG_HEADER_OK )
    {
        jpeg_destroy_decompress( &cinfo );        
        delete decodeStruct;
        kLogError( "Invalid JPG header" );
        return false;
    }
    if( cinfo.out_color_space != JCS_RGB && cinfo.out_color_space != JCS_GRAYSCALE )
    {
        jpeg_destroy_decompress( &cinfo );        
        delete decodeStruct;
        kLogError( "Unsupported JPG input format" );
        return false;
    }

    u32 bytesPerPixel = cinfo.out_color_space == JCS_RGB ? 3 : 1;

    // Assign image characteristics. 
    width       = (u32)cinfo.image_width;
    height      = (u32)cinfo.image_height;
    numChannels = bytesPerPixel;

    // Calculate row size, since RGB is assumed using value of 3. 
    u32 rowSize = width * bytesPerPixel;

    data = new u8 [rowSize * height];
    KI_ASSERT( data != NULL, "JPG memory allocation failed" );
    if( data == NULL )
    {
        delete decodeStruct;
        jpeg_destroy_decompress( &cinfo );        
        kLogError( "new u8 [%d] failed", rowSize * height );
        return false;
    }

    // Assign pointers for each row. 
    rowPointers = new u8* [height];
    KI_ASSERT( rowPointers != NULL, "OpenGL JPG memory allocation failed" );
    if( rowPointers == NULL )
    {
        delete [] data;
        delete [] decodeStruct;
        jpeg_destroy_decompress( &cinfo );        
        kLogError( "rowPointers = new u8* [%d] failed", height );
        return false;
    }

    // Assign pointers for each row. 
    u8* datap = data;

    for( u32 i = 0; i < height; ++i )
    {
        rowPointers[i] = datap;
        datap += rowSize;
    }

    // Prepare jpeg decompression. 
    if( !jpeg_start_decompress( &cinfo ) )
    {
        delete [] rowPointers;
        delete [] data;
        delete decodeStruct;
        jpeg_destroy_decompress( &cinfo );        
        kLogError( "Error in decompressing JPG image", height );
        return false;
    }

    // Read scan lines, data assigned via row pointers. 
    while( cinfo.output_scanline < cinfo.output_height )
    {
        s32 rowsRead = (s32)jpeg_read_scanlines( &cinfo, (JSAMPARRAY)&rowPointers[cinfo.output_scanline], 1 );
        KI_ASSERT( rowsRead > 0, "JPG reading image information failed" );
        if( rowsRead <= 0 )
        {
            delete [] rowPointers;
            delete [] data;
            delete decodeStruct;
            jpeg_destroy_decompress( &cinfo );        
            kLogError( "No rows were able to be read from JPG image" );
            return false;
        }
    }

    delete [] rowPointers;

    // Finish decompressing. 
    if( !jpeg_finish_decompress( &cinfo ))
    {
        delete [] data;
        delete decodeStruct;
        jpeg_destroy_decompress(&cinfo);        
        kLogError( "JPG decompression error in finishing" );
        return false;
    }

    // Free source manager helper structure. 
    delete decodeStruct;

    // Destroy decompress structure. 
    jpeg_destroy_decompress( &cinfo );

    fclose( fp );

    return true;
}

bool Image::WriteJPG( const char* _path )
{
    FILE* fp = std::fopen( _path, "wb" );
    if ( fp == NULL )
    {
        kLogError( "Failed to open %s", _path );
        return false;
    }

    KI_ASSERT( numChannels == 3, "JPG only supports RGB");

    if( numChannels != 3 )
    {
        kLogError( "JPEG support RGB image format only." );
        return false;
    } 

    jpeg_error_mgr jerr;

    // TODO: do not use default jpeg error handler since it automatically calls exit() internally upon failure 
    // Set the error manager. 
    jpeg_compress_struct cinfo;
    cinfo.err = jpeg_std_error( &jerr );
    // Create compression structure. 
    jpeg_create_compress( &cinfo );

    // Allocate memory for destination manager. 
    KIJPGEncodeStruct* encodeStruct = new KIJPGEncodeStruct;

    // Set helper struct for compression structure. 
    cinfo.dest = (jpeg_destination_mgr*)encodeStruct;

    // Setup write parameters for image. 
    encodeStruct->outputFp                 = fp;
    encodeStruct->dmgr.init_destination    = ImageJPGEncodeInitDestination;
    encodeStruct->dmgr.empty_output_buffer = ImageJPGEncodeEmptyBuffer;
    encodeStruct->dmgr.term_destination    = ImageJPGEncodeTermDestination;

    // Prepare for compression. Ignore alpha channel.
    cinfo.image_width      = (JDIMENSION)width;
    cinfo.image_height     = (JDIMENSION)height;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;

    // Set JPEG default after the color space has been defined. 
    jpeg_set_defaults( &cinfo );
    jpeg_set_quality( &cinfo, (s32)JPEG_QUALITY, TRUE );
    jpeg_start_compress( &cinfo, 1 );

    // Write the JPEG scan lines, one by one. 
    JSAMPROW sampleRow[1];
    JDIMENSION rowsWritten;

    while( cinfo.next_scanline < cinfo.image_height )
    {
        u8* imageRow;
        imageRow = (u8*)data + width * 3 * (u32)cinfo.next_scanline;

        sampleRow[0] = (JSAMPROW)&imageRow[0];
        rowsWritten = jpeg_write_scanlines( &cinfo, (JSAMPARRAY)sampleRow, 1 );
        if( rowsWritten != 1 )
        {
            delete encodeStruct;
            jpeg_destroy_compress( &cinfo );
            kLogError( "Invalid amount of rows written when outputting JPG image." );
            return false;
        }
    }

    // Finish compression. 
    jpeg_finish_compress( &cinfo );
    // Destroy compression structure. 
    jpeg_destroy_compress( &cinfo );

    // Release helper structure from memory. 
    delete encodeStruct;

    fclose( fp );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////// 
// PNG read and write
///////////////////////////////////////////////////////////////////////////////////// 

static void ImagePNGReadFunction( png_structp _png_ptr, png_bytep _data, png_size_t _length )
{
    FILE* fp = (FILE*)png_get_io_ptr(_png_ptr);

    fread( _data, _length, 1, fp );
}

static void ImagePNGWriteFunction( png_structp _png_ptr, png_bytep _data, png_size_t _length )
{
    FILE* fp = (FILE*)png_get_io_ptr( _png_ptr );

    u32 bytesWritten = fwrite( _data, 1, _length, fp );
    KI_ASSERT( bytesWritten == _length, "OpenGL PNG doesn't read required bytes." );
}

static void ImagePNGFlushFunction( png_structp _png_ptr )
{
    /* This function has been intentionally left empty. */
}

static png_voidp ImagePNGMallocFunction( png_structp _png_ptr, png_size_t _size )
{
    png_voidp allocatedBlock = new u8 [_size];
    KI_ASSERT(allocatedBlock != NULL, "OpenGL PNG memory allocation failed" );

    return allocatedBlock;
}

static void ImagePNGFreeFunction( png_structp _png_ptr, png_voidp _data )
{
    u8* p = (u8*)_data;
    delete [] p;
}

bool Image::ReadPNG( const char* _path )
{
    FILE* fp = fopen( _path, "rb" );
    if( fp == NULL )
    {
        kLogError( "Could not read %s.", _path );
        return false;
    }

    u8** rowPointers = NULL;

    // LibPNG specific structures. 
    png_structp png_ptr = NULL;
    png_infop info_ptr  = NULL;
    png_infop end_ptr   = NULL;
    png_byte pbSig[8];
    png_uint_32 png_result;

    if( !fread( (u8*)pbSig, 8, 1, fp ) )
    {
        return false;
    }

    // Check that we're reading valid PNG file. 
    if (png_sig_cmp(pbSig, 0, 8))
    {
        kLogError( "Not a valid PNG image (incorrect signature)." );
        return false;
    }

    // Our png_ptr is used by the libpng library to maintain, basically, state
    // information about the PNG file as it is being read in by the library.
    // It's used for housekeeping by the libpng library.  The other two
    // pointers, info_ptr and end_ptr, are used to help us extract data from
    // the PNG file. 
    png_ptr = png_create_read_struct_2(
            PNG_LIBPNG_VER_STRING, 
            NULL, // error function pointer 
            NULL, // error printing function
            NULL, // warning function
            NULL, // memory pointer
            ImagePNGMallocFunction, // memory allocation function pointer 
            ImagePNGFreeFunction ); // memory deallocation function pointer
    if( png_ptr == NULL )
    {
        kLogError( "Could not create a PNG reading structure" );
        return false;
    }

    if( setjmp( png_jmpbuf( png_ptr ) ) ) 
    {
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_ptr );
        delete [] data;
        delete [] rowPointers;
        return false;
    }

    // Create png info structure.
    info_ptr = png_create_info_struct( png_ptr );
    if( info_ptr == NULL )
    {
        png_destroy_read_struct( &png_ptr, (png_infopp)NULL, (png_infopp)NULL );
        kLogError( "Could not create PNG info structure" );
        return false;
    }

    end_ptr = png_create_info_struct( png_ptr );
    if( end_ptr == NULL ) 
    {
        png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)NULL );
        kLogError( "Could not create PNG info structure" );
        return false;
    }

    png_set_read_fn( png_ptr, (png_voidp)fp, ImagePNGReadFunction );
    png_set_sig_bytes( png_ptr, 8 );

    // Read png info. 
    png_read_info( png_ptr, info_ptr );

    // Read image info (width, height, color format) and data. 
    s32 bitDepth;
    s32 colorType;
    u32 channelCount;

    // Fetch image width, height, bit depth, color type. 
    png_result = png_get_IHDR( png_ptr, info_ptr, (png_uint_32*)&width, 
            (png_uint_32*)&height, &bitDepth, &colorType, NULL, NULL, NULL );
    if( png_result == 0 )
    {
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_ptr );
        kLogError( "PNG image has invalid header" );
        return false;
    }

    // See how many channels this png contains. 
    channelCount = (u32)png_get_channels( png_ptr, info_ptr );

    // Non-paletted image. 
    if( colorType != PNG_COLOR_TYPE_PALETTE )
    {
        // Support only for 8 bit depth pixels. 
        if( bitDepth == 8 )
        {
            numChannels = channelCount;
        }
        else
        {
            png_destroy_read_struct( &png_ptr, &info_ptr, &end_ptr );
            kLogError( "Unsupported PNG pixel format" );
            return false;
        }
    }
    else
    {
        // Paletted image. 
        numChannels = 3;
    }

    if( bitDepth == 16 )
    {
        png_set_strip_16( png_ptr );
        bitDepth = 8;
    }

    u32 rowSize = width * channelCount * (u32)bitDepth / 8;

    // Create row pointers for libpng and unpack the data to new buffers, finally assigning to image. 
    data = new u8 [rowSize * height];
    KI_ASSERT( data != NULL, "PNG memory allocation failed" );
    if( data == NULL)
    {
        kLogError( "new u8 [%d] failed", rowSize * height );
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_ptr );
        return false;
    }

    // Assign pointers for each row. 
    rowPointers = new u8*[height];
    KI_ASSERT( rowPointers != NULL, "PNG memory allocation failed." );
    if( rowPointers == NULL )
    {
        delete [] data;
        kLogError( "rowPointers = new u8* [%d] failed", height );
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);
        return false;
    }

    u8* datap = data;
    for( u32 i = 0; i < height; ++i )
    {
        rowPointers[i] = datap;

        datap += rowSize;
    }

    // Read image data using libpng.
    png_read_image( png_ptr, (png_bytepp)rowPointers );
    png_read_end( png_ptr, NULL );

    delete [] rowPointers;

    /* Convert to RGB data in case of paletted png. */
    if( colorType == PNG_COLOR_TYPE_PALETTE )
    {
        png_colorp palette;
        s32 numPalette;

        png_result = png_get_PLTE( png_ptr, info_ptr, &palette, &numPalette );
        if( png_result == 0 )
        {
            delete [] data;
            png_destroy_read_struct( &png_ptr, &info_ptr, &end_ptr );
            kLogError( "Invalid PNG palette" );
            return false;
        }

        u8* convertedImage = new u8 [width * height * 3];
        KI_ASSERT( convertedImage != NULL, "OpenGL PNG memory allocation failed" );
        if( convertedImage == NULL )
        {
            delete [] data;
            png_destroy_read_struct( &png_ptr, &info_ptr, &end_ptr );
            kLogError( "convertedImage = new u8 [%d] failed", height * width * 3 );
            return false;
        }

        // 4-bit palette, assign 2 pixels at a time. 
        if( bitDepth == 4 )
        {
            u32 offset = 0;
            u32 imageSize = width * channelCount * (u32)bitDepth * height / 8;

            // If both width and height are odd, no support for loading. 
            if( (width & 1) != 0 && (height & 1) != 0 )
            {
                delete [] data;
                delete [] convertedImage;
                png_destroy_read_struct( &png_ptr, &info_ptr, &end_ptr );
                kLogError( "4bit paletted png image with width and height having odd values not supported" );
                return false;
            }

            for( u32 i = 0; i < imageSize; ++i )
            {
                u32 highByte = (u32)( data[i] >> 4 );
                u32 lowByte = (u32)( data[i] & 0x0F );
                convertedImage[offset++] = (u8)palette[highByte].red;
                convertedImage[offset++] = (u8)palette[highByte].green;
                convertedImage[offset++] = (u8)palette[highByte].blue;
                convertedImage[offset++] = (u8)palette[lowByte].red;
                convertedImage[offset++] = (u8)palette[lowByte].green;
                convertedImage[offset++] = (u8)palette[lowByte].blue;
            }
        }
        // 8 bit palette, assign colors by fetching from palette. 
        else if(  bitDepth == 8 )
        {
            u32 offset = 0;
            u32 imageSize = rowSize * height;
            for( u32 i = 0; i < imageSize; i++ )
            {
                u8 colorValue = data[i];
                convertedImage[offset++] = (u8)palette[colorValue].red;
                convertedImage[offset++] = (u8)palette[colorValue].green;
                convertedImage[offset++] = (u8)palette[colorValue].blue;
            }
        }
        else
        {
            delete [] data;
            delete [] convertedImage;
            png_destroy_read_struct( &png_ptr, &info_ptr, &end_ptr );
            kLogError( "Invalid paletted image format" );
            return false;
        }

        delete [] data;
        data = convertedImage;
    }

    png_destroy_read_struct( &png_ptr, &info_ptr, &end_ptr );

    fclose( fp );

    return true;
}

bool Image::WritePNG( const char* _path )
{
    FILE* fp = fopen( _path, "wb" );
    if( fp == NULL)
    {
        kLogError( "Couldn't open %s to write.", _path );
        return false;
    }

    KI_ASSERT( numChannels== 3 || numChannels == 4, "PNG only supports RGB and RGBA." );
    if( numChannels != 3 && numChannels != 4)
    {
        kLogError( "Unsupported pixel format by PNG." );
        return false;
    }

    /* Create the write struct for png. */
    png_structp png_ptr = png_create_write_struct_2(
            PNG_LIBPNG_VER_STRING, 
            NULL, 
            NULL, 
            NULL, 
            NULL, 
            ImagePNGMallocFunction, 
            ImagePNGFreeFunction );
    if( png_ptr == NULL )
    {
        kLogError( "Error in writing PNG image." );
        return false;
    }

    // Create the info struct for png. Info struct contains info such as width,
    // height, bit depth and other attributes of image. 
    png_infop info_ptr = png_create_info_struct( png_ptr );
    if( info_ptr == NULL )
    {
        png_destroy_write_struct( &png_ptr, (png_infopp)NULL );
        kLogError( "Error in writing PNG image." );
        return false;
    }

    png_set_write_fn( png_ptr, (png_voidp)fp, ImagePNGWriteFunction, ImagePNGFlushFunction );

    // Set png to non-interlaced, non-compressed RGB mode. 
    png_set_IHDR( png_ptr, info_ptr, (png_uint_32)width, (png_uint_32)height, 8,
            numChannels == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );

    png_write_info( png_ptr, info_ptr );

    // Write the image data. 
    u32 rowSize = width * numChannels;

    u8** rowPointers = new u8* [height];
    KI_ASSERT( rowPointers != NULL, "PNG memory allocation failed" );
    if( rowPointers == NULL )
    {
        kLogError(" rowPointers = new u8* [%d] failed", height );
        png_destroy_write_struct( &png_ptr, &info_ptr );
        return false;
    }

    u8* datap = const_cast<u8*>( data );

    for( u32 i = 0; i < height; ++i )
    {
        rowPointers[i] = datap;
        datap += rowSize;
    }

    png_write_image( png_ptr, (png_bytepp)rowPointers );
    png_write_end( png_ptr, NULL );

    png_destroy_write_struct( &png_ptr, &info_ptr );

    delete [] rowPointers;

    fclose( fp );

    return true;
}

