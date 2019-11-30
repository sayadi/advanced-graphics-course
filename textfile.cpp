#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <cstring>

char *textFileRead(const char *fn) {


	FILE *fp;
	char *content = nullptr;

	int count=0;

	if (fn != nullptr)
	{
		fp = fopen(fn,"rt");

		if (fp != nullptr)
    {
      
        fseek(fp, 0, SEEK_END);
        count = ftell(fp);
        rewind(fp);

        if (count > 0)
        {
            content = (char *)malloc(sizeof(char) * (count+1));
            count = fread(content,sizeof(char),count,fp);
            content[count] = '\0';
        }
			fclose(fp);
		}
	}
	return content;
}

int textFileWrite(char *fn, char *s)
{

	FILE *fp;
	int status = 0;

	if (fn != nullptr)
	{
		fp = fopen(fn,"w");

		if (fp != nullptr)
		{
			
			if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
				status = 1;
			fclose(fp);
		}
	}
	return(status);
}
