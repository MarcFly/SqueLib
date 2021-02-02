#include <squelib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


bool SQUE_LOAD_Texture(SQUE_Asset* tex_bytes, SQUE_Texture2D* texture)
{
    if (tex_bytes == NULL || texture == NULL)
        return false;
    texture->pixels = stbi_load_from_memory((unsigned char*)tex_bytes->raw_data, tex_bytes->size, &texture->w, &texture->h, &texture->channel_num, 0);
    SQUE_SendTextureToGPU(*texture);
    stbi_image_free(texture->pixels);
    
    return true;
}