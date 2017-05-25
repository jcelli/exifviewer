#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*exif reader                                            *
 *reads exif data from different jpegs and prints data   *
 *Written by Joseph Patricelli                           *
 *CS449                                                  *
 *Tuesday/Thursday 4:00 P.M.                             *
 *Recitation Monday 2:00 P.M.                            */


 //define struct for tiff tags
 struct tiffTag
 {
 	short type;
 	int numItems;
 	int offset;
 	char text[40];
 	int num1;
 	int num2;
 };

 int main(int argc, char **argv)
 {
 	short check;
 	int count = 0;
 	struct tiffTag manufacturer;
 	struct tiffTag camera;
 	struct tiffTag moreExif;
 	struct tiffTag width;
 	struct tiffTag height;
 	struct tiffTag iso;
 	struct tiffTag exposure;
 	struct tiffTag fStop;
 	struct tiffTag lensFocalLength;
 	struct tiffTag date;

 	//put name of file into a string
 	char* pic = argv[1];
 	char text[40];
 	
 	FILE *ab;
 	ab = fopen(pic, "rb");

 	fread(&text, 1, 20, ab);

 	//Check to see if picture is not APP0 format
 	if(text[2] != 0xffffffff || text[3] != 0xffffffe1)
 	{
 		printf("The picture is not APP0 format. Ending\n");
 		exit(0);
 	}

 	//find count value
 	fseek(ab, 20, SEEK_SET);
 	fread(&count, 1, 2, ab);

 	int i;
 	for(i = 0; i <= count; i++)
 	{	
 		fread(&check, 1, 2, ab);
 		//check if the check is the beginning of manufacturing tag
 		if(check == 0x010F)
 		{
 			fread(&manufacturer.type, 1, 2, ab);
 			fread(&manufacturer.numItems, 1, 4, ab);
 			fread(&manufacturer.offset, 1, 4, ab);
  		}
 		//check if check is beginning of camera tag
 		else if(check == 0x0110)
 		{
 			fread(&camera.type, 1, 2, ab);
 			fread(&camera.numItems, 1, 4, ab);
 			fread(&camera.offset, 1, 4, ab);
 		}
 		//check if check leads to more exif data
 		else if(check == 0xffff8769)
 		{	
 			fread(&moreExif.type, 1, 2, ab);
 			fread(&moreExif.numItems, 1, 4, ab);
 			fread(&moreExif.offset, 1, 4, ab);
 		}
 		else
 		{
 			fseek(ab, 10, SEEK_CUR);
 		}
 	}

 	//go to block of addition exif data
 	fseek(ab, moreExif.offset + 12, SEEK_SET);
 	fread(&count, 1, 2, ab);

 	//loop through again looking for more tags
 	if(moreExif.type == 4)
 	{
 		for(i = 0; i <= count; i++)
 		{	
 			fread(&check, 1, 2, ab);
 			if(check == 0xffffA002)
 			{
 				fread(&width.type, 1, 2, ab);
 				fread(&width.numItems, 1, 4, ab);
 				fread(&width.offset, 1, 4, ab);
 			}
 			else if(check == 0xffffA003)
 			{
 				fread(&height.type, 1, 2, ab);
 				fread(&height.numItems, 1, 4, ab);
 				fread(&height.offset, 1, 4, ab);
 			}
 			else if(check == 0xffff8827)
 			{
 				fread(&iso.type, 1, 2, ab);
 				fread(&iso.numItems, 1, 4, ab);
 				fread(&iso.offset, 1, 3, ab);
 				fseek(ab, 1, SEEK_CUR);
 			}
 			else if(check == 0xffff829a)
 			{
 				fread(&exposure.type, 1, 2, ab);
 				fread(&exposure.numItems, 1, 4, ab);
 				fread(&exposure.offset, 1, 4, ab);
 			}
 			else if(check == 0xffff829d)
 			{
 				fread(&fStop.type, 1, 2, ab);
 				fread(&fStop.numItems, 1, 4, ab);
 				fread(&fStop.offset, 1, 4, ab);
 			}
 			else if(check == 0xffff920A)
 			{
 				fread(&lensFocalLength.type, 1, 2, ab);
 				fread(&lensFocalLength.numItems, 1, 4, ab);
 				fread(&lensFocalLength.offset, 1, 4, ab);
 			}
 			else if(check == 0xffff9003)
 			{
 				fread(&date.type, 1, 2, ab);
 				fread(&date.numItems, 1, 4, ab);
 				fread(&date.offset, 1, 4, ab);
 			}
 			else
 			{
 				fseek(ab, 10, SEEK_CUR);
 			}
 		}
 	}

 	//print out manufacturer data
 	fseek(ab, manufacturer.offset + 12, SEEK_SET);
 	fread(&manufacturer.text, 1, manufacturer.numItems, ab);
 	printf("Manufacturer:	%s\n", manufacturer.text);
          
 	//print out camera model data
 	fseek(ab, camera.offset + 12, SEEK_SET);
 	fread(&camera.text, 1, camera.numItems, ab);
 	printf("Model:       	%s\n", camera.text);

 	if(moreExif.type == 4)
 	{
 		fseek(ab, exposure.offset + 12, SEEK_SET);
 		fread(&exposure.num1, 1, 4, ab);
 		fread(&exposure.num2, 1, 4, ab);
 		printf("Exposure time:	%d/%d second\n", exposure.num1, exposure.num2);

 		fseek(ab, fStop.offset + 12, SEEK_SET);
 		fread(&fStop.num1, 1, 4, ab);
 		fread(&fStop.num2, 1, 4, ab);
 		printf("F-stop:         f/%2.1f\n", (double)fStop.num1 / (double)fStop.num2);

 		printf("ISO:            ISO %d\n", iso.offset);

 		fseek(ab, date.offset + 12, SEEK_SET);
 		fread(&date.text, 1, date.numItems, ab);
 		printf("Date Taken:     %s\n", date.text);

 		fseek(ab, lensFocalLength.offset + 12, SEEK_SET);
 		fread(&lensFocalLength.num1, 1, 4, ab);
 		fread(&lensFocalLength.num2, 1, 4, ab);
 		printf("Focal Length:   %d mm\n", lensFocalLength.num1/lensFocalLength.num2);

 		printf("Width:       	%d pixels\n", width.offset);
 		printf("Height:         %d pixels\n", height.offset);
 	}
 }
 
