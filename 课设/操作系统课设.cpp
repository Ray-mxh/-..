#include "stdio.h"
// 字符串处理库 - 提供字符串操作函数
#include "string.h"
// 控制台输入输出库 - 提供getch()等控制台操作函数
#include "conio.h"
// 标准库 - 提供内存分配、进程控制等功能
#include "stdlib.h"
// C++标准输入输出流 - 提供cout、cin等流操作
#include <iostream>
// 文件流库 - 提供文件读写功能
#include <fstream>
// C++字符串类 - 提供更方便的字符串处理
#include <string>
// C++风格的C字符串库 - 兼容C风格字符串操作
#include <cstring>
// 动态数组容器 - 提供动态数组功能
#include <vector>
// C++风格的标准库 - 提供通用工具函数
#include <cstdlib>

// 使用标准命名空间，避免每次都写std::前缀
using namespace std; 

// 定义常量
#define MAXNAME 25  /* 最大文件名长度，包括MFD、UFD和普通文件名 */
#define MAXCHILD 50 /* 单个目录下允许的最大子项数量 */
/* 文件物理地址编号的最大值，基于子项数量的平方计算 */
#define MAX (MAXCHILD*MAXCHILD)

typedef struct  /*the structure of OSFILE*/
{
	int  fpaddr;                /*file physical address*/
    int  flength;               /*file length*/
    int  fmode;   /*file mode:0-Read Only;1-Write Only;2-Read and Write; 3-Protect;*/
    char fname[MAXNAME];        /*file name*/
} OSFILE;

typedef struct     /*the structure of OSUFD*/
{
	char ufdname[MAXNAME];   /*ufd name*/
	OSFILE ufdfile[MAXCHILD];   /*ufd own file*/
}OSUFD;

typedef struct  /*the structure of OSUFD'LOGIN*/
{
	char ufdname[MAXNAME];       /*ufd name*/
    char ufdpword[8];            /*ufd password*/
} OSUFD_LOGIN;

typedef struct     /*file open mode*/
{
	int ifopen;     /*ifopen:0-close,1-open*/
    int openmode;   /*0-read only,1-write only,2-read and write,3-initial*/
}OSUFD_OPENMODE;

OSUFD *ufd[MAXCHILD];   /*ufd and ufd own files*/
OSUFD_LOGIN ufd_lp;

int ucount=0;  /*the count of mfd's ufds*/
int fcount[MAXCHILD];  /*the count of ufd's files*/
int loginsuc=0; /*whether login successfully*/
char username[MAXNAME];  /*record login user's name22*/
char dirname[MAXNAME];/*record current directory*/
int fpaddrno[MAX];  /*record file physical address num*/
OSUFD_OPENMODE ifopen[MAXCHILD][MAXCHILD]; /*record file open/close*/
int wgetchar; /*whether getchar()*/

FILE *fp_mfd,*fp_ufd,*fp_file_p,*fp_file;

void LoginF();  /*LOGIN FileSystem*/
void DirF();  /*Dir FileSystem*/
void CdF();  /*Change Dir*/
void CreateF();  /*Create File*/
void DeleteF(); /*Delete File*/
void ModifyFM(); /*Modify FileMode*/
void OpenF();  /*Open File*/
void CloseF();  /*Close File*/
void ReadF(); /*Read File*/
void WriteF(); /*Write File*/
void QuitF(); /*Quit FileSystem*/
void help();

char *rtrim(char *str);  /*remove the trailing blanks.*/
char *ltrim(char *str);  /*remove the heading blanks.*/
void InputPW(char *password);  /*input password,use '*' replace*/
void SetPANo(int RorW);  /*Set physical address num*/
int ExistD(char *dirname);  /*Whether DirName Exist,Exist-i,Not Exist-0*/
int WriteF1(); /*write file*/
int ExistF(char *filename);  /*Whether FileName Exist,Exist-i,Not Exist-0*/
int FindPANo();  /*find out physical address num*/


 
void clrscr()
{
	system("cls");
}

int main()
{
	int i,choice1;
	char choice[50];  /*choice operation:dir,create,delete,open,delete,modify,read,write*/
	int choiceend=1;  /*whether choice end*/
	char *rtrim(char *str);  /*remove the trailing blanks.*/
	char *ltrim(char *str);  /*remove the heading blanks.*/

	if((fp_mfd=fopen("c:\\osfile\\mfd.txt","rb"))==NULL)
	{
		fp_mfd=fopen("c:\\osfile\\mfd.txt","wb");
		fclose(fp_mfd);
	}

	for(i=0;i<MAX;i++) fpaddrno[i]=0;
	//textattr(BLACK*16|WHITE);
	clrscr();   /*clear screen*/
	LoginF();   /*user login*/
	clrscr();

	if(loginsuc==1)  /*Login Successfully*/
	{
		while (1)
		{
			wgetchar=0;
			if (choiceend==1)
				printf("\n\nC:\\%s>",strupr(dirname));
			else 
				printf("Bad command or file name.\nC:\\%s>",strupr(username));
		    gets(choice);
		    strcpy(choice,ltrim(rtrim(strlwr(choice))));
		    if (strcmp(choice,"dir")==0) choice1=1;
		    else if(strcmp(choice,"create")==0) choice1=2;
		    else if(strcmp(choice,"delete")==0) choice1=3;
		    else if(strcmp(choice,"attrib")==0) choice1=4;
		    else if(strcmp(choice,"open")==0) choice1=5;
		    else if(strcmp(choice,"close")==0) choice1=6;
		    else if(strcmp(choice,"read")==0) choice1=7;
		    else if(strcmp(choice,"write")==0) choice1=8;
		    else if(strcmp(choice,"exit")==0) choice1=9;
		    else if(strcmp(choice,"cls")==0) choice1=10;
		    else if(strcmp(choice,"cd")==0) choice1=11;
		    else if(strcmp(choice,"help")==0) choice1=20;
		    else choice1=12;

			switch(choice1)
			{
				case 1:DirF();choiceend=1;break;
				case 2:CreateF();choiceend=1;if(!wgetchar) getchar();break;
				case 3:DeleteF();choiceend=1;if(!wgetchar)getchar();break;
				case 4:ModifyFM();choiceend=1;if(!wgetchar) getchar();break;
				case 5:OpenF();choiceend=1;if (!wgetchar) getchar();break;
				case 6:CloseF();choiceend=1;if (!wgetchar) getchar();break;
				case 7:ReadF();choiceend=1;if (!wgetchar) getchar();break;
				case 8:WriteF();choiceend=1;if (!wgetchar) getchar();break;
				case 9:printf("\nYou have exited this system.");
					   QuitF();exit(0);break;
				case 10:clrscr();choiceend=1;break;
				case 11:CdF();choiceend=1;break;
				case 20:help();choiceend=1;break;
				default:choiceend=0;
			}
		}	
	}
	else 
		printf("\nAccess denied.");
return 0; 
}

void help(void)
{
	printf("\nThe Command List\n");
	printf("\nCd  Attrib  Create  write  Read  Open  Cls  Delete  Exit  Close\n");
}

char *rtrim(char *str)  /*remove the trailing blanks.*/
{
	int n=strlen(str)-1;
	while(n>=0)
	{
		if(*(str+n)!=' ')
		{
			*(str+n+1)='\0';
			break;
		}
		else n--;
	}
	if (n<0) str[0]='\0';
	return str;
}

char *ltrim(char *str) /*remove the heading blanks.*/
{
	strrev(str);
	rtrim(str);
	strrev(str);
	return str;
}

void LoginF()  /*LOGIN FileSystem*/
{
	char loginame[MAXNAME],loginpw[9],logincpw[9],str[50];
	int i,j,flag=1;
	char a[25];
	int findout; /*login user not exist*/

	while(1)
	{
		findout=0;
		printf("\n\nLogin Name:");
		gets(loginame);
		ltrim(rtrim(loginame));
		fp_mfd=fopen("c:\\osfile\\mfd.txt","rb");
		for(i=0;fread(&ufd_lp,sizeof(OSUFD_LOGIN),1,fp_mfd)!=0;i++)
			if (strcmp(strupr(ufd_lp.ufdname),strupr(loginame))==0)
			{
				findout=1;
				strcpy(logincpw,ufd_lp.ufdpword);
			}
		fclose(fp_mfd);
		if (findout==1)  /*user exist*/
		{
			printf("Login Password:");
			InputPW(loginpw); /*input password,use '*' replace*/
			if (strcmp(loginpw,logincpw)==0)
			{
				strcpy(username,strupr(loginame));
				strcpy(dirname,username);
				fp_mfd=fopen("c:\\osfile\\mfd.txt","rb");
				for(j=0;fread(&ufd_lp,sizeof(OSUFD_LOGIN),1,fp_mfd)!=0;j++)
				{
					strcpy(str,"c:\\osfile\\");
					strcat(str,ufd_lp.ufdname);
					strcat(str,".txt");
					ufd[j]=(OSUFD*)malloc(sizeof(OSUFD));
					strcpy(ufd[j]->ufdname,strupr(ufd_lp.ufdname));
					fp_ufd=fopen(str,"rb");
					fcount[j]=0;
					for(i=0;fread(&ufd[j]->ufdfile[i],sizeof(OSFILE),1,fp_ufd)!=0;i++,fcount[j]++)
					{
						ifopen[j][i].ifopen=0;
						ifopen[j][i].openmode=4;
					}
					fclose(fp_ufd);
				}
			
				fclose(fp_mfd);
				ucount=j;
				
				SetPANo(0);
				printf("\n\nLogin successful! Welcome to this FileSystem\n\n");
				loginsuc=1;
				return;
			}
			else
			{
				printf("\n\n");
				flag=1;
				while(flag)
				{
					printf("Login Failed!  Password Error.  Try Again(Y/N):");
					gets(a);
					ltrim(rtrim(a));
					if (strcmp(strupr(a),"Y")==0) 
					{
						loginsuc=0;
						flag=0;
					}
					else if(strcmp(strupr(a),"N")==0)
					{
						loginsuc=0;
						flag=0;
						return;
					}
				}
			}
		}
		else
		{
			printf("New Password(<=8):");
			InputPW(loginpw); /*input new password,use '*' replace*/
			printf("\nConfirm Password(<=8):"); /*input new password,use '*' replace*/
			InputPW(logincpw);
			if (strcmp(loginpw,logincpw)==0)
			{
				strcpy(ufd_lp.ufdname,strupr(loginame));
				strcpy(ufd_lp.ufdpword,loginpw);
				fp_mfd=fopen("c:\\osfile\\mfd.txt","ab");
				fwrite(&ufd_lp,sizeof(OSUFD_LOGIN),1,fp_mfd);
				fclose(fp_mfd);
				strcpy(username,strupr(loginame));
				strcpy(dirname,loginame);

				////////////////////////////////////////////////////////
				strcpy(str,"c:\\osfile\\");
				strcat(str,username);
				strcat(str,".txt");
				if((fp_ufd=fopen(str,"rb"))==NULL)
				{
					fp_ufd=fopen(str,"wb");
					fclose(fp_ufd);
				}
				fp_mfd=fopen("c:\\osfile\\mfd.txt","rb");
				for(j=0;fread(&ufd_lp,sizeof(OSUFD_LOGIN),1,fp_mfd)!=0;j++)
				{
					/////////////////////////////////////
					strcpy(str,"c:\\osfile\\");
					strcat(str,ufd_lp.ufdname);
					strcat(str,".txt");
					ufd[j]=(OSUFD*)malloc(sizeof(OSUFD));
					strcpy(ufd[j]->ufdname,strupr(ufd_lp.ufdname));
					fp_ufd=fopen(str,"rb");
					for(i=0;fread(&ufd[j]->ufdfile[i],sizeof(OSFILE),1,fp_ufd)!=0;i++,fcount[j]++)
					{
						ifopen[j][i].ifopen=0;
					    ifopen[j][i].openmode=4;
					}
					fclose(fp_ufd);
				}
				fclose(fp_mfd);
				ucount=j;
				SetPANo(0);
				printf("\n\nLogin Successful! Welcome to this System\n\n");
				loginsuc=1;
				return;
			}
		    else
			{
				printf("\n\n");
				flag=1;
				while(flag)
				{
					printf("Login Failed! Password Error. Try Again(Y/N):");
					gets(a);
					ltrim(rtrim(a));
					if (strcmp(strupr(a),"Y")==0) 
					{
						loginsuc=0;
						flag=0;
					}
					else if(strcmp(strupr(a),"N")==0)
					{
						loginsuc=0;
						flag=0;
						return;
					}
				}
			}
		}
	}
}

void SetPANo(int RorW)  /*Set physical address num,0-read,1-write*/
{
	int i,j;
	if (RorW==0)
	{
		if((fp_file_p=fopen("c:\\osfile\\file\\file_p.txt","rb"))==NULL)
	    {
			fp_file_p=fopen("c:\\osfile\\file\\file_p.txt","wb");
			fclose(fp_file_p);
	    }
		fp_file_p=fopen("c:\\osfile\\file\\file_p.txt","rb");

		///////////////////////////////////////////////////////////////////
		for(i=0;fread(&j,sizeof(int),1,fp_file_p)!=0;i++)
			fpaddrno[j]=1;

		/*for(i=1;i<MAX;i++)
			if ((i%13)==0) 
				fpaddrno[i]=1;*/
	}
	else
	{
		fp_file_p=fopen("c:\\osfile\\file\\file_p.txt","wb");
		/*for(i=1;i<MAX;i++)
			if((i%13)==0) 
				fpaddrno[i]=0;*/
		for(i=0;i<MAX;i++)
			if (fpaddrno[i]==1)
				fwrite(&i,sizeof(int),1,fp_file_p);
	}
	fclose(fp_file_p);
}

void InputPW(char *password)  /*input password,use '*' replace*/
{
	int j;
	for(j=0;j<=7;j++)
	{	
		password[j]=getch();
		if ((int)(password[j])!=13)
		{
			if((int)(password[j])!=8)
			putchar('*');
			else
			{
				if (j>0)
				{
					j--;
					j--;
					putchar('\b');putchar(' ');putchar('\b');
				}
				else j--;
			}
		}
		else
		{	password[j]='\0';
			break;
		}
	}
	password[j]='\0';
}

void DirF()  /*Dir FileSystem*/
{
	int i,j,count=0;
	char sfmode[25],sfpaddr[25],str[25];
	
	clrscr();
	if (strcmp(strupr(ltrim(rtrim(dirname))),"")!=0)
	{
		printf("\n\nC:\\%s>dir\n",dirname);
		printf("\n%14s%16s%14s%10s%18s\n","FileName","FileAddress","FileLength","Type","FileMode");
		j=ExistD(dirname);
		for(i=0;i<fcount[j];i++)
		{
			if ((i%16==0)&&(i!=0))
			{
				printf("\nPress any key to continue..");
				getch();
				clrscr();
				printf("\n%14s%16s%14s%10s%18s\n","FileName","FileAddress","FileLength","Type","FileMode");
			}
			/////////////////////////////////////////////////////////
			itoa(ufd[j]->ufdfile[i].fpaddr,str,10);
			strcpy(sfpaddr,"file");
			strcat(sfpaddr,str);
			if (ufd[j]->ufdfile[i].fmode==0) strcpy(sfmode,"Read Only");
			else if(ufd[j]->ufdfile[i].fmode==1) strcpy(sfmode,"Write Only");
			else if(ufd[j]->ufdfile[i].fmode==2)strcpy(sfmode,"Read And Write");
			else strcpy(sfmode,"Protect");
			printf("%14s%16s%14d%10s%18s\n",ufd[j]->ufdfile[i].fname,sfpaddr,ufd[j]->ufdfile[i].flength,"<FILE>",sfmode);
		}
		printf("\n %3d file(s)\n",fcount[j]);
	}
	else
	{
		printf("\n\nC:\\>dir\n");
		printf("\n%14s%18s%8s\n","DirName","OwnFileCount","Type");
		for(i=0;i<ucount;i++)
		{
			if ((i%16==0)&&(i!=0))
			{
				printf("\nPress any key to continue...");
				getch();
				clrscr();
				printf("\n%14s%18s%8s\n","DirName","OwnFileCount","Type");
			}
			printf("%14s%18d%8s\n",ufd[i]->ufdname,fcount[i],"<UFD>");
			count=count+fcount[i];
		}
		printf("\n %3d dir(s),%5d file(s)\n",ucount,count);
	}
}

int ExistD(char *dirname)  /*Whether DirName Exist,Exist-i,Not Exist-0*/
{
	int i;
	int exist=0;
	for(i=0;i<ucount;i++)
		if (strcmp(strupr(ufd[i]->ufdname),strupr(dirname))==0)
		{
			exist=1;
			break;
		}
	if (exist) return(i);
	else return(-1);
}

void CdF() /*Exchange Dir*/
{
	char dname[MAXNAME];

	printf("\nPlease input DirName (cd..-Previous dir; DirNAME-cd [DirNAME]):");
	gets(dname);
	ltrim(rtrim(dname));
	if (ExistD(dname)>=0)  strcpy(dirname,strupr(dname));
	else 
		if(strcmp(strupr(dname),"CD..")==0)  strcpy(ltrim(rtrim(dirname)),"");
		else printf("\nError.\'%s\' does not exist.\n",dname);
}

void CreateF()  /*Create File*/
{
	int fpaddrno,flag=1,i;
	char fname[MAXNAME],str[50],str1[50],a[25];
	char fmode[25];

 
	if (strcmp(strupr(dirname),strupr(username))!=0)
	{
		printf("\nError. You must create file in your own dir.\n");
		wgetchar=1;
	}
	else
	{
		printf("\nPlease input FileName:");
		gets(fname);
		ltrim(rtrim(fname));
		if (ExistF(fname)>=0)
		{
			printf("\nError. Name \'%s\' has already existed.\n",fname);
			wgetchar=1;
		}
		else
		{
			printf("Please input FileMode(0-Read Only, 1-Write Only, 2-Read and Write, 3-Protect):");
			gets(fmode);
			ltrim(rtrim(fmode));
			if((strcmp(fmode,"0")==0)||(strcmp(fmode,"1")==0)||(strcmp(fmode,"2")==0)||(strcmp(fmode,"3")==0))
			{
				fpaddrno=FindPANo();
				if (fpaddrno>=0)
				{
					i=ExistD(username);
					strcpy(ufd[i]->ufdfile[fcount[i]].fname,fname);
					ufd[i]->ufdfile[fcount[i]].fpaddr=fpaddrno;
					ufd[i]->ufdfile[fcount[i]].fmode=atoi(fmode);
					ifopen[i][fcount[i]].ifopen=0;
					ifopen[i][fcount[i]].openmode=4;
					strcpy(str,"c:\\osfile\\file\\file");
					itoa(fpaddrno,str1,10);
					strcat(str,str1);
					strcat(str,".txt");
					fp_file=fopen(str,"wb");
					fclose(fp_file);
					fcount[i]++;
					while(flag)
					{
						printf("Input text now(Y/N):");
						gets(a);
						ltrim(rtrim(a));
						ufd[i]->ufdfile[fcount[i]-1].flength=0;
						if(strcmp(strupr(a),"Y")==0)
						{
							fp_file=fopen(str,"wb+");
							ufd[i]->ufdfile[fcount[i]-1].flength=WriteF1();
							flag=0;
						}
						else if(strcmp(strupr(a),"N")==0)
						{
							flag=0;
							wgetchar=1;
						}
					}
					printf("\n\'%s\' has been created successfully!\n",fname);
				}
				else
				{
					printf("\nFail!No Disk Space. Please format your disk.\n");
					wgetchar=1;
				}
			}
			else 
			{	
				printf("\nError. FileMode\'s Range is 0-3\n");
				wgetchar=1;
			}
		}
	}
}

int ExistF(char *filename)  /*Whether FileName Exist,Exist-i,Not Exist-0*/
{
	int i,j;
	int exist=0;

	j=ExistD(dirname);
	for(i=0;i<fcount[j];i++)
	if (strcmp(strupr(ufd[j]->ufdfile[i].fname),strupr(filename))==0)
	{
		exist=1;
		break;
	}
	if (exist) return(i);
	else return(-1);
}

int FindPANo()  /*find out physical address num*/
{
	int i;
	for(i=0;i<MAX;i++)
	if (fpaddrno[i]==0) 
	{
		fpaddrno[i]=1;
		break;
	}
	if (i<MAX) return(i);
	else return(-1);
}

int WriteF1()  /*write file*/
{
	int length=0;
	char c;
	printf("Please input text(\'#\' stands for end):\n");
	while((c=getchar())!='#')
	{
		fprintf(fp_file,"%c",c);
		if (c!='\n') length++;
	}
	fprintf(fp_file,"\n");
	fclose(fp_file);
	return(length);
}

// 文件删除功能 - 实现文件的彻底删除，包括物理文件和目录记录
void DeleteF()
{
    char fname[MAXNAME];           // 存储用户输入的文件名
    
    // 步骤1：提示用户输入要删除的文件名
    std::cout << "\n=== 文件删除功能 ===" << endl;
    std::cout << "请输入要删除的文件名: ";
    std::cin.getline(fname, MAXNAME);
    ltrim(rtrim(fname));           // 去除文件名前后的空白字符
    
    // 步骤2：检查当前目录是否存在
    // ExistD函数通过目录名查找UFD索引，-1表示目录不存在
    int ufdIndex = ExistD(dirname);
    if (ufdIndex < 0) {
        std::cout << "\n错误: 当前目录 '" << dirname << "' 不存在!" << endl;
        wgetchar = 1;              // 设置等待用户按键标志
        return;
    }
    
    // 步骤3：检查文件是否存在于当前目录中
    // ExistF函数通过文件名查找文件索引，-1表示文件不存在
    int fileIndex = ExistF(fname);
    if (fileIndex < 0) {
        std::cout << "\n错误: 文件 '" << fname << "' 不存在!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 步骤4：检查文件是否已打开
    // ifopen数组记录文件打开状态：0-关闭，1-打开
    if (ifopen[ufdIndex][fileIndex].ifopen == 1) {
        std::cout << "\n错误: 文件 '" << fname << "' 已打开，无法删除!" << endl;
        std::cout << "请先使用 close 命令关闭文件。" << endl;
        wgetchar = 1;
        return;
    }
    
    // 步骤5：确认用户是否真的要删除文件
    // 重要操作二次确认，防止误删除
    char confirm;
    std::cout << "\n警告: 您确定要删除文件 '" << fname << "' 吗? (Y/N): ";
    std::cin >> confirm;
    std::cin.ignore(); // 清除输入缓冲区，避免残留字符影响后续输入
    
    if (confirm != 'Y' && confirm != 'y') {
        std::cout << "\n操作已取消。" << endl;
        wgetchar = 1;
        return;
    }
    
    // 步骤6：释放物理地址
    // 从UFD中获取文件的物理地址，并标记该地址为可用(0表示未使用)
    int physAddr = ufd[ufdIndex]->ufdfile[fileIndex].fpaddr;
    fpaddrno[physAddr] = 0;        // 更新物理地址分配表
    std::cout << "-> 已释放物理地址 #" << physAddr << endl;
    
    // 步骤7：删除物理文件
    // 构建物理文件路径并调用remove函数删除实际文件
    char filePath[100];
    sprintf(filePath, "c:\\osfile\\file\\file%d.txt", physAddr);
    if (remove(filePath) != 0) {
        // 删除失败处理，可能是文件被其他进程锁定或权限不足
        std::cout << "警告: 物理文件删除失败!" << endl;
        std::cout << "文件路径: " << filePath << endl;
        std::cout << "请检查文件权限或手动删除。" << endl;
    } else {
        std::cout << "-> 物理文件已成功删除" << endl;
    }
    
    // 步骤8：从目录中移除文件记录
    // 通过数组前移覆盖要删除的文件记录
    for (int i = fileIndex; i < fcount[ufdIndex] - 1; i++) {
        ufd[ufdIndex]->ufdfile[i] = ufd[ufdIndex]->ufdfile[i + 1];
        ifopen[ufdIndex][i] = ifopen[ufdIndex][i + 1];
    }
    fcount[ufdIndex]--;            // 更新目录文件计数
    
    // 步骤9：保存目录更改
    // 将更新后的目录信息写入磁盘文件永久保存
    char dirPath[100];
    sprintf(dirPath, "c:\\osfile\\%s.txt", ufd[ufdIndex]->ufdname);
    FILE* fp = fopen(dirPath, "wb"); // 以二进制写模式打开
    if (fp != NULL) {
        // 将目录中的所有文件信息写入文件
        for (int i = 0; i < fcount[ufdIndex]; i++) {
            fwrite(&ufd[ufdIndex]->ufdfile[i], sizeof(OSFILE), 1, fp);
        }
        fclose(fp);
        std::cout << "-> 目录更新已保存" << endl;
    } else {
        std::cout << "警告: 目录更新保存失败!" << endl;
    }
    
    // 步骤10：显示删除成功信息
    std::cout << "\n=== 文件删除成功 ===" << endl;
    std::cout << "文件名: " << fname << endl;
    std::cout << "物理地址: " << physAddr << endl;
    std::cout << "当前目录文件数量: " << fcount[ufdIndex] << endl;
    wgetchar = 1;                  // 等待用户按键确认
}

void ModifyFM() /*Modify FileMode*/
{
	char fname[MAXNAME],str[50];
	int i,k;
	char fmode[25]; /*whether delete*/
		
	if (strcmp(strupr(dirname),strupr(username))!=0) 
	{
		printf("\nError.You can only modify filemode in yourself dir.\n");
		wgetchar=1;
	}
	else
	{ 
		printf("\nPlease input FileName:");
		gets(fname);
		ltrim(rtrim(fname));
		i=ExistF(fname);
		if (i>=0)
		{
			k=ExistD(username);
			if(ifopen[k][i].ifopen==1)
			{
				printf("\nError.\'%s\' is in open status. Close it before modify.\n",fname);
				wgetchar=1;
			}
			else
			{
				if(ufd[k]->ufdfile[i].fmode==0) strcpy(str,"read only");    /*FileMode*/
				else if(ufd[k]->ufdfile[i].fmode==1) strcpy(str,"write only");
				else if(ufd[k]->ufdfile[i].fmode==2) strcpy(str,"read and write");
				else strcpy(str,"Protect");

				printf("\'%s\' filemode is %s.\n",fname,strupr(str));
				printf("Modify to(0-read only,1-write only,2-read and write,3-Protect):");
				gets(fmode);
				ltrim(rtrim(fmode));
				if(strcmp(fmode,"0")==0)
				{
					ufd[k]->ufdfile[i].fmode=0;
					printf("\n\'%s\' has been modified to READ ONLY mode successfully.\n",fname);
					wgetchar=1;
				}
				else if(strcmp(fmode,"1")==0)
				{
					ufd[k]->ufdfile[i].fmode=1;
					printf("\n\'%s\' has been modified to WRITE ONLY mode successfully.\n",fname);
					wgetchar=1;
				}
				else if(strcmp(fmode,"2")==0)
				{
					ufd[k]->ufdfile[i].fmode=2;
					printf("\n\'%s\' has been modified to READ AND WRITE mode successfully.\n",fname);
					wgetchar=1;
				}
				else if(strcmp(fmode,"3")==0)
				{
					ufd[k]->ufdfile[i].fmode=3;
					printf("\n\'%s\' has been modified to FORBID mode successfully.\n",fname);
					wgetchar=1;
				}
				else 
				{
					printf("\nError.\'%s\' is not modified.\n",fname);
					wgetchar=1;
				}
			}
		}
		else
		{
			printf("\nError. \'%s\' dose not exist.\n",fname);
			wgetchar=1;
		}
	}
}


// 文件打开功能 - 实现文件的打开操作，包括权限检查和模式验证
void OpenF() {
    char fname[MAXNAME];           // 存储用户输入的文件名
    char modeStr[10];              // 存储用户输入的打开模式字符串
    int openMode;                  // 存储转换后的打开模式整数
    
    // 获取用户输入的文件名
    cout << "\n请输入要打开的文件名: ";
    cin.getline(fname, MAXNAME);   // 读取包含空格的文件名
    ltrim(rtrim(fname));           // 去除文件名前后的空白字符
    
    // 检查当前目录是否存在
    int ufdIndex = ExistD(dirname);
    if (ufdIndex < 0) {
        cout << "\n错误: 当前目录不存在!" << endl;
        wgetchar = 1;              // 设置等待用户按键标志
        return;
    }
    
    // 检查文件是否存在于当前目录中
    int fileIndex = ExistF(fname);
    if (fileIndex < 0) {
        cout << "\n错误: 文件 '" << fname << "' 不存在!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 检查文件权限模式
    // 0: 只写, 1: 只读, 2: 读写, 3: 保护模式(不可打开)
    int fileMode = ufd[ufdIndex]->ufdfile[fileIndex].fmode;
    if (fileMode == 3) { // 保护模式
        cout << "\n错误: 文件 '" << fname << "' 是保护模式，无法打开!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 检查文件是否已在系统中打开
    if (ifopen[ufdIndex][fileIndex].ifopen == 1) {
        cout << "\n错误: 文件 '" << fname << "' 已打开!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 获取用户指定的打开模式
    cout << "请输入打开模式 (0-只读, 1-只写, 2-读写): ";
    cin.getline(modeStr, 10);
    ltrim(rtrim(modeStr));
    openMode = atoi(modeStr);      // 将字符串转换为整数
    
    // 权限验证逻辑
    // 检查读权限: 只读(0)或读写(2)模式需要文件允许读操作
    if ((openMode == 0 || openMode == 2) && fileMode == 1) {
        cout << "\n错误: 文件不允许读操作!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 检查写权限: 只写(1)或读写(2)模式需要文件允许写操作
    if ((openMode == 1 || openMode == 2) && fileMode == 0) {
        cout << "\n错误: 文件不允许写操作!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 文件打开成功流程
    // 标记文件为已打开状态并记录打开模式
    ifopen[ufdIndex][fileIndex].ifopen = 1;
    ifopen[ufdIndex][fileIndex].openmode = openMode;
    
    cout << "\n文件 '" << fname << "' 已成功打开!" << endl;
    wgetchar = 1;
}

// 文件关闭功能 - 实现已打开文件的关闭操作，释放文件资源
void CloseF() {
    char fname[MAXNAME];           // 存储用户输入的文件名
    
    // 获取用户输入的要关闭的文件名
    cout << "\n请输入要关闭的文件名: ";
    cin.getline(fname, MAXNAME);   // 读取包含空格的文件名
    ltrim(rtrim(fname));           // 去除文件名前后的空白字符
    
    // 检查当前目录是否存在
    int ufdIndex = ExistD(dirname);
    if (ufdIndex < 0) {
        cout << "\n错误: 当前目录不存在!" << endl;
        wgetchar = 1;              // 设置等待用户按键标志
        return;
    }
    
    // 检查文件是否存在于当前目录中
    int fileIndex = ExistF(fname);
    if (fileIndex < 0) {
        cout << "\n错误: 文件 '" << fname << "' 不存在!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 检查文件是否处于打开状态
    // ifopen数组记录文件打开状态：0-关闭，1-打开
    if (ifopen[ufdIndex][fileIndex].ifopen == 0) {
        cout << "\n错误: 文件 '" << fname << "' 未打开!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 文件关闭流程
    // 标记文件为关闭状态
    ifopen[ufdIndex][fileIndex].ifopen = 0;
    // 设置特殊模式值4表示文件已关闭（可作为日志追踪标记）
    ifopen[ufdIndex][fileIndex].openmode = 4;
    
    // 输出成功信息并等待用户确认
    cout << "\n文件 '" << fname << "' 已成功关闭!" << endl;
    wgetchar = 1;
}

// 文件读取功能 - 实现已打开文件的内容读取操作
void ReadF() {
    char fname[MAXNAME];           // 存储用户输入的文件名
    
    // 获取用户输入的要读取的文件名
    cout << "\n请输入要读取的文件名: ";
    cin.getline(fname, MAXNAME);   // 读取包含空格的文件名
    ltrim(rtrim(fname));           // 去除文件名前后的空白字符
    
    // 检查当前目录是否存在
    int ufdIndex = ExistD(dirname);
    if (ufdIndex < 0) {
        cout << "\n错误: 当前目录不存在!" << endl;
        wgetchar = 1;              // 设置等待用户按键标志
        return;
    }
    
    // 检查文件是否存在于当前目录中
    int fileIndex = ExistF(fname);
    if (fileIndex < 0) {
        cout << "\n错误: 文件 '" << fname << "' 不存在!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 检查文件是否已打开
    // ifopen数组记录文件打开状态：0-关闭，1-打开
    if (ifopen[ufdIndex][fileIndex].ifopen == 0) {
        cout << "\n错误: 文件 '" << fname << "' 未打开!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 检查文件打开模式是否允许读取
    // 0-只读，1-只写，2-读写
    if (ifopen[ufdIndex][fileIndex].openmode == 1) {
        cout << "\n错误: 文件以只写模式打开，无法读取!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 构建物理文件路径
    // 文件存储在c:\osfile\file目录下，使用文件物理地址作为文件名
    char filePath[100];
    sprintf(filePath, "c:\\osfile\\file\\file%d.txt", 
           ufd[ufdIndex]->ufdfile[fileIndex].fpaddr);
    
    // 打开物理文件进行读取
    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "\n错误: 无法打开物理文件!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 逐行读取并输出文件内容
    cout << "\n文件内容:\n";
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    
    // 关闭文件流并等待用户确认
    file.close();
    wgetchar = 1;
}

// 文件写入功能 - 实现已打开文件的内容写入操作
void WriteF() {
    char fname[MAXNAME];           // 存储用户输入的文件名
    char content[1024];            // 存储用户输入的写入内容
    
    // 获取用户输入的要写入的文件名
    cout << "\n请输入要写入的文件名: ";
    cin.getline(fname, MAXNAME);   // 读取包含空格的文件名
    ltrim(rtrim(fname));           // 去除文件名前后的空白字符
    
    // 检查当前目录是否存在
    int ufdIndex = ExistD(dirname);
    if (ufdIndex < 0) {
        cout << "\n错误: 当前目录不存在!" << endl;
        wgetchar = 1;              // 设置等待用户按键标志
        return;
    }
    
    // 检查文件是否存在于当前目录中
    int fileIndex = ExistF(fname);
    if (fileIndex < 0) {
        cout << "\n错误: 文件 '" << fname << "' 不存在!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 检查文件是否已打开
    // ifopen数组记录文件打开状态：0-关闭，1-打开
    if (ifopen[ufdIndex][fileIndex].ifopen == 0) {
        cout << "\n错误: 文件 '" << fname << "' 未打开!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 检查文件打开模式是否允许写入
    // 0-只读，1-只写，2-读写
    if (ifopen[ufdIndex][fileIndex].openmode == 0) {
        cout << "\n错误: 文件以只读模式打开，无法写入!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 构建物理文件路径
    // 文件存储在c:\osfile\file目录下，使用文件物理地址作为文件名
    char filePath[100];
    sprintf(filePath, "c:\\osfile\\file\\file%d.txt", 
           ufd[ufdIndex]->ufdfile[fileIndex].fpaddr);
    
    // 获取用户输入的写入内容
    cout << "请输入要写入的内容 (输入'#'结束):\n";
    cin.getline(content, 1024);
    
    // 打开文件进行写入操作
    ofstream file;
    if (ifopen[ufdIndex][fileIndex].openmode == 1) {
        // 只写模式(1)：覆盖原有内容
        file.open(filePath, ios::trunc);
    } else {
        // 读写模式(2)：追加到文件末尾
        file.open(filePath, ios::app);
    }
    
    if (!file.is_open()) {
        cout << "\n错误: 无法打开物理文件!" << endl;
        wgetchar = 1;
        return;
    }
    
    // 写入内容直到遇到结束符'#'
    for (int i = 0; content[i] != '\0'; i++) {
        if (content[i] == '#') break;  // 遇到'#'终止写入
        file << content[i];             // 写入单个字符
        // 更新文件长度（字节数）
        ufd[ufdIndex]->ufdfile[fileIndex].flength++;
    }
    
    // 关闭文件流并提示成功
    file.close();
    cout << "\n内容已成功写入文件!" << endl;
    wgetchar = 1;
}

void QuitF() /*Quit FileSystem*/
{
	int i,j;
	char str[50];
	
	SetPANo(1);
	if (fp_mfd!=NULL) fclose(fp_mfd);
	if (fp_ufd!=NULL) fclose(fp_ufd);
	if (fp_file!=NULL) fclose(fp_file);
	for(j=0;j<ucount;j++)
	{
		strcpy(str,"c:\\osfile\\");
		strcat(str,ufd[j]->ufdname);
		ltrim(rtrim(str));
		strcat(str,".txt");
		fp_ufd=fopen(str,"wb");
		fclose(fp_ufd);
		fp_ufd=fopen(str,"ab");
		for(i=0;i<fcount[j];i++)
			fwrite(&ufd[j]->ufdfile[i],sizeof(OSFILE),1,fp_ufd);
		fclose(fp_ufd);
	}

}
