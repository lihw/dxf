#ifndef _IMAGE_H
#define _IMAGE_H

#define u32 unsigned int
#define u8 unsigned char

struct Image
{
    Image();
    ~Image();

    void Create( u32 _width, u32 _height, u32 _numChannels, u8* _data = NULL );
    bool Read( const char* _path );
    bool Write( const char* _path );

    u32 width;
    u32 height;
    u32 numChannels;
	u8* data;
    
private:
    bool ReadPNG( const char* _path );
    bool WritePNG( const char* _path );
    bool ReadJPG( const char* _path );
    bool WriteJPG( const char* _path );
};

#endif // _IMAGE_H
