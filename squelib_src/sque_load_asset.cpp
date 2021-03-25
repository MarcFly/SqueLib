#include <squelib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


bool SQUE_LOAD_Texture(SQUE_Asset* tex_bytes, SQUE_Texture* texture)
{
    char* data = (char*)stbi_load_from_memory((unsigned char*)tex_bytes->raw_data, tex_bytes->size, &texture->w, &texture->h, &texture->channel_num, 0);
    delete tex_bytes->raw_data;
    tex_bytes->raw_data = data;
    // Here size will not be aligned with actual texture size probably...
    return true;
}

void SQUE_FREE_Texture(SQUE_Asset* tex_bytes)
{
    stbi_image_free(tex_bytes->raw_data);
}