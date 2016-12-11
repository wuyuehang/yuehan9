#ifndef _RENDER_UTILITY_H_
#define _RENDER_UTILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_shader_from_file(const char *filepath, char **content)
{
	FILE *fp = fopen(filepath, "rb");
	long length = -1;

	if (NULL == fp) {
		printf("filepath: %s doesn't exist\n", filepath);
		return;
	}

	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	*content = (char *)malloc(length + 1);

	if (NULL == *content) {
		printf("out of memory\n");
		goto exit;
	}

	memset(*content, 0 , length +1);

	if (length != fread(*content, 1, length, fp))
		printf("fail to read all content\n");

exit:
	fclose(fp);
}

#endif
