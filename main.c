#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>

#include "LodePng/lodepng.h"

void decodeImage(const char *filename);                                                              // Decode and get image data
void encodeImage(const char *filename, const unsigned char *image, unsigned width, unsigned height); // Encode and save image

int main(int argc, char *argv[])
{

  if (argc > 1)
  {

    for (int i = 1; i < argc; i++)
    {
      const char *filename = argv[i];
      char *file_extension = strrchr(filename, '.');
      if (strcmp(file_extension, ".png") == 0)
      {
        decodeImage(filename);
      }
    }
  }
  else
  {

    printf("image path : ");
    char *filename;
    scanf("%s", filename);
    char *file_extension = strrchr(filename, '.');
    if (strcmp(file_extension, ".png") == 0)
    {
      decodeImage(filename);
    }
    
  }

  return 0;
}

/* Load PNG file from disk to memory first, then decode to raw pixels in memory. */
void decodeImage(const char *filename)
{
  unsigned error;
  unsigned char *image = 0;
  unsigned width, height;
  unsigned char *png = 0;
  size_t pngsize;

  error = lodepng_load_file(&png, &pngsize, filename);
  if (!error)
    error = lodepng_decode32(&image, &width, &height, png, pngsize);
  if (error)
    printf("error %u: %s\n", error, lodepng_error_text(error));

  free(png);

  // Find the minimum and maximum x and y coordinates of the non-transparent pixels
  unsigned min_x = width - 1;
  unsigned min_y = height - 1;
  unsigned max_x = 0;
  unsigned max_y = 0;
  for (unsigned y = 0; y < height; y++)
  {
    for (unsigned x = 0; x < width; x++)
    {
      if (image[(y * width + x) * 4 + 3] != 0)
      { // Check alpha channel
        if (x < min_x)
          min_x = x;
        if (y < min_y)
          min_y = y;
        if (x > max_x)
          max_x = x;
        if (y > max_y)
          max_y = y;
      }
    }
  }

  // Crop the image to the non-transparent region
  unsigned char *cropped_image = malloc((max_x - min_x + 1) * (max_y - min_y + 1) * 4);
  for (unsigned y = min_y; y <= max_y; y++)
  {
    for (unsigned x = min_x; x <= max_x; x++)
    {
      cropped_image[((y - min_y) * (max_x - min_x + 1) + (x - min_x)) * 4 + 0] =
          image[(y * width + x) * 4 + 0];
      cropped_image[((y - min_y) * (max_x - min_x + 1) + (x - min_x)) * 4 + 1] =
          image[(y * width + x) * 4 + 1];
      cropped_image[((y - min_y) * (max_x - min_x + 1) + (x - min_x)) * 4 + 2] =
          image[(y * width + x) * 4 + 2];
      cropped_image[((y - min_y) * (max_x - min_x + 1) + (x - min_x)) * 4 + 3] =
          image[(y * width + x) * 4 + 3];
    }
  }

  // rename new file name
  char *prefix = "new_";
  char *flname = basename(filename);
  char *NewName = malloc(strlen(prefix) + strlen(flname) + 1);
  strcpy(NewName, prefix);
  strcat(NewName, flname);

  // Encode and save new image
  encodeImage(NewName, cropped_image, max_x - min_x + 1, max_y - min_y + 1);

  // Free image and cropped image
  free(image);
  free(cropped_image);

}

void encodeImage(const char *filename, const unsigned char *image, unsigned width, unsigned height)
{
  /*Encode the image*/
  unsigned error = lodepng_encode32_file(filename, image, width, height);

  /*if there's an error, display it*/
  if (error)
    printf("error %u: %s\n", error, lodepng_error_text(error));
}