#define FALSE 0
#define TRUE 1
void printHello();
void printString(char*);
void readString(char*);
int mod(int, int);
int div(int, int);
void readSector(char*, int);
void handleInterrupt21(int, int, int, int);
void readFile(char*, char*);
void executeProgram(char*, int);
void terminate();
void writeSector(char* , int);
void deleteFile(char*);
void writeFile(char*,char*,int);

main()
{
	
		//test task 1
		/*char buffer[512];
		char buff[512];
		readSector(buffer, 30);   //dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc after copying this
		writeSector(buffer, 31);
		readSector(buff, 31);
		printString(buff);
		*/

		//test task 2
		/*char buffer[13312];
		makeInterrupt21();
		interrupt(0x21, 7, "messag\0", 0, 0); //delete messag
		interrupt(0x21, 3, "messag\0", buffer, 0); // try to read messag
		interrupt(0x21, 0, buffer, 0, 0); //print out the contents of buffer
		*/

		//test task 3
		/*int i=0;
		char buffer1[13312];
		char buffer2[13312];
		buffer2[0]='h'; 
		buffer2[1]='e';
		buffer2[2]='l';
		buffer2[3]='l';
		buffer2[4]='o';
		for(i=5; i<13312; i++)
		 buffer2[i]=0x0;
		makeInterrupt21();
		interrupt(0x21,8, "testW\0", buffer2, 1); //write file testW
		interrupt(0x21,3, "testW\0", buffer1, 0); //read file testW
		interrupt(0x21,0, buffer1, 0, 0); // print out contents of testW*/

		//test task 3
		makeInterrupt21(); 
        interrupt(0x21, 4, "shell\0", 0x2000, 0);

		while(TRUE);
 }

void printHello()
{
	interrupt(0x10, 0xE*256+'H', 0, 0, 0);
	interrupt(0x10, 0xE*256+'E', 0, 0, 0);
	interrupt(0x10, 0xE*256+'L', 0, 0, 0);
	interrupt(0x10, 0xE*256+'L', 0, 0, 0);
	interrupt(0x10, 0xE*256+'O', 0, 0, 0);
	interrupt(0x10, 0xE*256+' ', 0, 0, 0);
	interrupt(0x10, 0xE*256+'W', 0, 0, 0);
	interrupt(0x10, 0xE*256+'O', 0, 0, 0);
	interrupt(0x10, 0xE*256+'R', 0, 0, 0);
	interrupt(0x10, 0xE*256+'L', 0, 0, 0);
	interrupt(0x10, 0xE*256+'D', 0, 0, 0);
	interrupt(0x10, 0xE*256+'!', 0, 0, 0);
}

void printString(char* chars)
{
	int i = 0;
	while(chars[i] != '\0')
	{
		interrupt(0x10, 0xE*256+chars[i], 0, 0, 0);
		i++;
	}
}

void readString(char* buffer)
{
	int backspace = 0x8;
	int enter = 0xd;
	int endOfString = 0x0;
	int lineFeed = 0xa;
	int count = 0;

	while(TRUE)
	{
		int ascii = interrupt(0x16, 0, 0, 0, 0);
		if(ascii == enter)
		{
			buffer[count] = endOfString;
			buffer[count+1] = enter;
			buffer[count+2] = lineFeed;
			interrupt(0x10,0xE*256+endOfString,0,0,0);
			interrupt(0x10,0xE*256+enter,0,0,0);
			interrupt(0x10,0xE*256+lineFeed,0,0,0);
			return;
		}
		else
			if(ascii == backspace)
			{
				if(count > 0)
				{
					count--;
					interrupt(0x10,0xE*256+backspace,0,0,0);
          			interrupt(0x10,0xE*256+endOfString,0,0,0);
          			interrupt(0x10,0xE*256+backspace,0,0,0);
				}
			}
			else
			{
				buffer[count] = ascii;
				interrupt(0x10, 0xE*256+ascii, 0, 0, 0);
				count++;
			}
	}
}

int mod(int n1, int n2)
{
	while (n1 >= n2)
		n1 -= n2;
	return n1;
}

int div(int n1, int n2)
{
	int count = 0;
	while(n1 >= n2)
	{
		count++;
		n1 -= n2;
	}
	return count;
}

void readSector(char* buffer, int sector)
{
	int relativeSector = mod(sector, 18) + 1;
	int head = mod(div(sector, 18), 2);
	int track = div(sector, 36);
	interrupt(0x13, 2*256+1, buffer, track*256+relativeSector, head*256+0);
}

void writeSector(char* buffer, int sector)
{
	int relativeSector = mod(sector, 18) + 1;
	int head = mod(div(sector, 18), 2);
	int track = div(sector, 36);
	interrupt(0x13, 3*256+1, buffer, track*256+relativeSector, head*256+0);
}

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
	switch (ax)
	{
		case 0: printString(bx); break;
		case 1: readString(bx); break;
		case 2: readSector(bx, cx); break;
		case 3: readFile(bx, cx); break;
		case 4: executeProgram(bx, cx); break;
		case 5: terminate(); break;
		case 6 : writeSector(bx,cx);break;
		case 7 : deleteFile(bx);break;
		case 8 : writeFile(bx,cx,dx);break;
	}
}

void readFile(char* fileName, char* buffer)
{	
	char directory[512];
	int i;
	readSector(directory, 2);
	for(i=0; i<512; i+=32)
	{
		int matched=1;
		int j;
		for(j=0; j<6; j++)
			if(fileName[j] == '\0')
				fileName[j] = 0x00;	

		for(j=0; j<6; j++)
			if(fileName[j] != directory[j+i])
			{
				matched=0;
				break;
			}

		if(matched == 1)
		{
			int bufferAddress=0;
			for(j=6; j<32 && directory[j+i] != 0x00; j++)
			{
				readSector(buffer+bufferAddress, directory[j+i]);
				bufferAddress+=512;
			}
			return;
		}
	}
}

void writeFile(char* name, char* buffer, int secNum){
    char directory[512];
	char map[512];
	int i;
	readSector(map, 1);
	readSector(directory, 2);
	for(i=0; i<512; i+=32)
	{
		int j;
		int k;
		int z=0;
		if(directory[i]== 0x00)
			{
			for(j=0;j<6;j++){
				if(name[j]!='\0'){
				   directory[i+j]=name[j];
				}
				else{
				   directory[i+j]=0x00;
				}	
			}
			i+=6;
			for(k=0;k<secNum ;k++){
				for(j=0;j<512;j++){
					if(map[j]==0x00){
						map[j]=0xFF;
						break;
					}
				}
				directory[i]=j;
				i++;
				writeSector(buffer+z,j);
				z+=512;
		   }
		   for(k=0;k<26-secNum;k++){
			   directory[i+k]=0x00;
		   }
		    writeSector(map,1);
			writeSector(directory,2);
			return;
		}	
	}
}

void deleteFile(char* name){
	char directory[512];
	char map[512];
	int i;
	char buffer[13312];
	readFile(name, buffer);
	if(buffer[0] == '\0')
	{
		 char string[16];
		 string[0] = 'F';
		 string[1] = 'i';
		 string[2] = 'l';
		 string[3] = 'e';
		 string[4] = ' ';
		 string[5] = 'n';
		 string[6] = 'o';
		 string[7] = 't';
		 string[8] = ' ';
		 string[9] = 'f';
		 string[10] = 'o';
		 string[11] = 'u';
		 string[12] = 'n';
		 string[13] = 'd'; 
		 string[14] = '!';
		 string[15] = '\0';
		interrupt(0x21, 0, string, 0, 0);
		return;
	}
	readSector(map, 1);
	readSector(directory, 2);
	for(i=0; i<512; i+=32)
	{
		int matched=1;
		int j;
		
		for(j=0; j<6; j++){
			if(name[j] == '\0')
				name[j] = 0x00;	
		}

		for(j=0; j<6; j++){
			if(name[j] != directory[j+i])
			{
				matched=0;
				break;
			}
		}

		if(matched == 1){
			directory[i]=0x00;
			for(j=6; j<32 && directory[j+i] != 0x00; j++)
			{
				map[directory[j+i]]=0x00;
			}
			writeSector(map,1);
			writeSector(directory,2);
			return;
		}

	}
}

void executeProgram(char* name, int segment)
{
	char buffer[13312];
	int i;
	readFile(name, buffer);
	if(buffer[0] == '\0')
	{
		 char string[16];
		 string[0] = 'F';
		 string[1] = 'i';
		 string[2] = 'l';
		 string[3] = 'e';
		 string[4] = ' ';
		 string[5] = 'n';
		 string[6] = 'o';
		 string[7] = 't';
		 string[8] = ' ';
		 string[9] = 'f';
		 string[10] = 'o';
		 string[11] = 'u';
		 string[12] = 'n';
		 string[13] = 'd'; 
		 string[14] = '!';
		 string[15] = '\0';
		interrupt(0x21, 0, string, 0, 0);
		return;
	}
	for(i=0; i<13312; i++)
		putInMemory(segment, i, buffer[i]);
	for(i = 0; i<6; i++)
        buffer[i] = '\0';
	launchProgram(segment);
}

void terminate()
{
	char shell[6];
	shell[0] = 's';
	shell[1] = 'h';
	shell[2] = 'e';
	shell[3] = 'l';
	shell[4] = 'l';
	shell[5] = '\0';
	interrupt(0x21, 4, shell, 0x2000, 0);
}

