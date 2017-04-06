#define FALSE 0
#define TRUE 1
void validateInput(char*);
int div(int , int);
int mod(int, int);
void printString(char*);

main()
{
    char buffer[180];
    while(TRUE)
    {
        interrupt(0x21, 0, "SHELL>", 0, 0);
        interrupt(0x21, 1, buffer, 0, 0);
        validateInput(buffer);
    }
}

void validateInput(char* buffer)
{
    char* command1 = "view ";
    char* command2 = "execute ";
    char* command3= "delete ";
    char* command4= "copy ";
    char* command5="dir";
    char* command6="create ";
    int i;
    char readBuffer[13312];
    int matchedView=1;
    int matchedExecute=1;
    int matchedDelete=1;
    int matchedCopy=1;
    int matchedDir=1;
    int matchedCreate=1;
    for(i=0; i<5; i++)
        if(buffer[i] != command1[i])
        {
            matchedView=0;
            break;
        }

    for(i=0; i<8; i++)
        if(buffer[i] != command2[i])
        {
            matchedExecute=0;
            break;
        }
     for(i=0; i<7; i++)
        if(buffer[i] != command3[i])
        {
            matchedDelete=0;
            break;
        }
        for(i=0; i<5; i++)
            if(buffer[i] != command4[i])
            {
                matchedCopy=0;
                break;
            }
        for(i=0;i<3;i++){
            if(buffer[i] != command5[i] || buffer[3]!='\0')
            {
                matchedDir=0;
                break;
            }
        }
        for(i=0;i<7;i++){
             if(buffer[i] != command6[i])
            {
                matchedCreate=0;
                break;
            }
        }
    
    if(matchedView)
    {
        interrupt(0x21, 3, buffer+5, readBuffer, 0);
        if(readBuffer[0] == '\0')
            interrupt(0x21, 0, "File does not exist!", 0, 0);
        else
            interrupt(0x21, 0, readBuffer, 0, 0);
    }
    else if(matchedExecute){
            interrupt(0x21, 4, buffer+8, 0x2000, 0);
    }
    else if (matchedDelete){
            interrupt(0x21, 3, buffer+7, readBuffer, 0);
             if(readBuffer[0] == '\0'){
            interrupt(0x21, 0, "File does not exist!", 0, 0);
             }
        else{
            interrupt(0x21, 7, buffer+7,0, 0);
        }
    }
    else if(matchedCopy){
        char filename1[6];
        char filename2[6];
        char readBuffer[13312];
        int flag=TRUE;
        int c1=0;

        for(i=5;buffer[i]!='\0';i++){
            if(buffer[i]=='\x20'){
                flag=FALSE;
                i++;
            }

            if(flag){
            filename1[c1]=buffer[i];
            c1++;
            }else{
                break;
            }
        }
        if(filename1[0]!='\0'){
             interrupt(0x21,3, filename1, readBuffer, 0);
            if(readBuffer[0]=='\0'){
            interrupt(0x21, 0, "File does not exist!!", 0, 0);
            }
            else{

            interrupt(0x21,8, buffer+i, readBuffer, 1);
            }
        }
        else{
            interrupt(0x21, 0, "Wrong  format", 0, 0);
        }

    } else if (matchedDir){
        char directory[512];
        char temp[3];
        int j;
        char filename[6];
        char size[10];
        int count;
        size[0]=0x30;
        size[1]=0x31;
        size[2]=0x32;
        size[3]=0x33;
        size[4]=0x34;
        size[5]=0x35;
        size[6]=0x36;
        size[7]=0x37;
        size[8]=0x38;
        size[9]=0x39;
  
        interrupt(0x21, 2, directory, 2, 0);
        for(i=0;i<512;i+=32){
            if(directory[i]!=0x00){
                for(j=0; j<6;j++){
                    if(directory[i+j]==0x00){
                      filename[j]='\0';
                    }
                    else{
                      filename[j]=directory[i+j];
                    }
                }
                    filename[6]='\0';
                    interrupt(0x21, 0, filename, 0, 0);
                    interrupt(0x21, 0, "\x20", 0, 0);
                count=0;
                for(j=6;j<32;j++){
                    if(directory[i+j]!=0x00){
                        count++;
                    }
                    else{
                        break;
                    }
                  }
                // temp[0] = (char) mod(count, 10);
                // printString(temp);
                //   count = div(count, 10);
                //   temp[1] = (char) mod(count, 10);
                //   temp[2] = '\0';

                // interrupt(0x21,0,temp,0,0);
                switch(count){
                    case 10: interrupt(0x21,0,"10",0,0);break;
                    case 11: interrupt(0x21,0,"11",0,0);break;
                    case 12: interrupt(0x21,0,"12",0,0);break;
                    case 13: interrupt(0x21,0,"13",0,0);break;
                    case 14: interrupt(0x21,0,"14",0,0);break;
                    case 15: interrupt(0x21,0,"15",0,0);break;
                    case 16: interrupt(0x21,0,"16",0,0);break;
                    case 17: interrupt(0x21,0,"17",0,0);break;
                    case 18: interrupt(0x21,0,"18",0,0);break;
                    case 19: interrupt(0x21,0,"19",0,0);break;
                    case 20: interrupt(0x21,0,"20",0,0);break;
                    case 21: interrupt(0x21,0,"21",0,0);break;
                    case 22: interrupt(0x21,0,"22",0,0);break;
                    case 23: interrupt(0x21,0,"23",0,0);break;
                    case 24: interrupt(0x21,0,"24",0,0);break;
                    case 25: interrupt(0x21,0,"25",0,0);break;
                    case 26: interrupt(0x21,0,"26",0,0);break;
                    default:interrupt(0x10,0xE*256+size[count],0,0,0);

                }
                interrupt(0x21, 0, "\0", 0, 0);
                interrupt(0x21, 0, "\n", 0, 0);
                interrupt(0x21, 0, "\x0D", 0, 0);            
                    }
        }
    }
    else if(matchedCreate){
       char fileContent[13312];
       char line[300];
       int k=0;
       int j=0;
       int secNum;
        while(TRUE){
           interrupt(0x21,0,">>\0",0,0);
           interrupt(0x21,1,line,0,0);
           if(line[0]!='\0'){
                for(k=0;line[k]!='\0';k++){
                    fileContent[j+k]=line[k];
                }
                // fileContent[j+k]='\0';
                //  k++;
                 fileContent[j+k]='\n';
                 k++;
                 fileContent[j+k]='\x0D';
                 k++;
                 j+=k;
        }
        else{
            secNum= div(j,512) ;
                secNum++;
            interrupt(0x21,8,buffer+7,fileContent,secNum);
            return;
        }
     }

    }
         else{
            interrupt(0x21, 0, "Bad Command", 0, 0);
         }
    
    interrupt(0x21, 0, "\0", 0, 0);
    interrupt(0x21, 0, "\n", 0, 0);
    interrupt(0x21, 0, "\x0D", 0, 0);
    for(i = 0; i<6; i++)
        readBuffer[i] = '\0';
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
int mod(int n1, int n2)
{
	while (n1 >= n2)
		n1 -= n2;
	return n1;
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
