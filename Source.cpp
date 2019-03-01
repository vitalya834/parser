
#include <stdlib.h>
#include <stdio.h> 
//#include <conio.h>  

//#include<bitset>
//#include<string>

#pragma warning(disable : 4996) //  Это нужно токльо для  VS си++
#define Max_Kol_Fra 1024 // Максимальное количество фреймов 

// typedef int bool ; // Это нужно столько для старого СИ
// const int true =1; // у него нету этого  
// const int false=0; // 

unsigned int POS = 0; // Указатель на читаемый байт 
unsigned int NOM = 0; // Номер последнего прочитаного фрейма   
unsigned int SIZ = 0; // Максимальное количество данных в потоке  
char*source; // Поток данных 

typedef struct sFrame {
	unsigned char      AC[4]; unsigned int Frame_Number; // Frame Header   8
	unsigned char      AB[8];                           // DDO Header     8
	unsigned long long DDO[7];                           // DDO Data       7*8 56
	unsigned char      DD[4]; unsigned char ART[4];     // Art Header +Art  8
	unsigned char      AD[8];                           // L0  Data Header  8
} sFrame; // Структура описывающая фрейм 


sFrame*Frames[Max_Kol_Fra];// Массив с фреймами 
int    FSizes[Max_Kol_Fra];// Рамер данных фремйма
//auto x = 1,23423423;
 
char *FileToStr(FILE *file) { // Переводит априоре существующий открытый  файл в строку 
	char *str = NULL;// НЕкая строка в памяти память под неё не выделена 
	int len = 0;// РАзмер файла в байтах 
	int real_len = 0;// реальная длина строки наверно типа того 
	int lines = 0;// Должно было быть количеситво строк     
	fseek(file, 0, SEEK_END);// перемещаемся в конец файла   
	len = ftell(file);// ПОлучаем размер файла в байтах 
	SIZ = len;// Запоминаем размер данных 
	fseek(file, 0, SEEK_SET);// Перемещаемся в начало файла 
	str = (char*)malloc(len + 128);// Выделяем память куда загрузим файл участок как строка 
	int Read_Byte;
	int kol = 0;
	do {
		Read_Byte = fread(&str[kol], 128, 1, file);// Загружаем файл
		kol = kol + 128;
	} 
	while (Read_Byte == 1);
		fclose(file);
	return str;// Возвращаем указатель на строку содержащию текст программы файл  
}

void  StrToFile(FILE *file, char* iStr) { // записывает в файл
	int kol = 0;
	int Write_Byte;
	fseek(file, 0, SEEK_SET);// Перемещаемся в начало файла 
	do {
		Write_Byte = fwrite(&iStr[kol], 128, 1, file);// Загружаем файл
		kol = kol + 128;
	} 
		while ((Write_Byte == 1) && (kol < SIZ));
	fclose(file);
}
bool  Find_Start_Frame() {
	bool ex = false;
	while (!ex&&SIZ > POS) {
		if ((*(unsigned long long*)&source[POS] == 0xbcbcbcbcbcbcbcbc) && (*(unsigned char*) &source[POS + 8] != 0xbc))
		{
			ex = true; 
			POS = POS + 8;
		}
		else POS++;
	}
	if (POS >= SIZ) return false; 
		else 
		return true;
}
bool  Find_Next_Frame() {
	unsigned int Kol_BC = 0;
	while (*(unsigned int*)&source[POS] != 0xACACACAC && SIZ > POS) {
		Kol_BC = Kol_BC + 1;
		POS = POS + 8;
	}
	FSizes[NOM - 1] = Kol_BC;
	if (POS >= SIZ) return false;		
	else 
		return true;
}
bool  Read_Frame() {
	Frames[NOM] = (sFrame*)&source[POS];
	// проверка фрейма
	int f = 0;
	bool Er = false;
	for (f = 0; f < 4; f++) 
		if (Frames[NOM]->AC[f] != 0xAC) 
			Er = true;
	for (f = 0; f < 8; f++) 
		if (Frames[NOM]->AB[f] != 0xAB) 
			Er = true;
	for (f = 0; f < 8; f++) 
		if (Frames[NOM]->AD[f] != 0xAD) 
			Er = true;
	for (f = 0; f < 4; f++) 
		if (Frames[NOM]->DD[f] != 0xDD) 
			Er = true;
	if (Er) printf("Frame %d format Error ", NOM);
	POS = POS + sizeof(sFrame);// Перемещаемся в конец фрейма 
	NOM++;// Увеличиваем номер фрейма 
	return !Er;
}

int main()
{

	// Сдесь Читаем Файл ======================================================
	FILE *f;// Файл с исходным кодом программы 
	if ((f = fopen("L0_085_sk_00__2715.txt", "rb")) != NULL) // Открытие и Проверка на существование файла 
		source = FileToStr(f); else
		printf("File not found");

	//=========================================================================

	// Чтение фреймов =========================================================
	POS = 0;// Устанавливаем указатель на начало данных 
	NOM = 0;// Количество фреймов 
	if (Find_Start_Frame()) // Ищим стартовый фрейм 
		while (Read_Frame() && Find_Next_Frame()); // находим все фреймы  
		// ========================================================================
	printf(" Count Frame %d \r\n", NOM);// Выводим количество фреймов 
	// ========================================================================
	FILE *fp;
	if ((fp = fopen("tes.txt", "w")) == NULL) {
		printf("Cannot open file.\n");
		exit(1);
		//retun 1
	}

//	fprintf(fp, "this is a test %d %f", 10, 20.01);
	//fclose(fp);




	int  Nom_Fram = 0; // Вывод фремйов 
	for (Nom_Fram = 0; Nom_Fram < NOM; Nom_Fram++) {
		//------------------------------------------------------------------------
		printf("=============================================================\r\n");
		printf("=============================================================\r\n");
		printf(" EVENT %d ", Frames[Nom_Fram]->Frame_Number);       // Выводим DDO
		printf(" DDO DATA \r\n");

//*******************************************************************
		fprintf(fp,"=============================================================\r\n");
		fprintf(fp,"=============================================================\r\n");
		fprintf(fp," EVENT %d ", Frames[Nom_Fram]->Frame_Number);       // Выводим DDO
		fprintf(fp," DDO DATA \r\n");

		int  n = 0; int b = 0;
		for (n = 0; n < 7; n++) 
		{
			for (b = 54; b >= 0; b = b - 6)
			{
				fprintf(fp, "%3d ", (Frames[Nom_Fram]->DDO[n] >> b) & 0x0000003F);
				printf("%3d ", (Frames[Nom_Fram]->DDO[n] >> b) & 0x0000003F);

			}
			printf("\r\n");

			fprintf(fp,"\r\n");

		
		}
		
	//***	***********************************************************************

		//------------------------------------------------------------------------
		unsigned int buf_part; 
		//int32_t buf_part;
		//std::bitset<32> buf_part;
	
		// выводим заголовок 
		printf("ART: %d \r\n", Frames[Nom_Fram]->ART[0]);
		printf("Header: \r\n");

		fprintf(fp, "ART: %d \r\n", Frames[Nom_Fram]->ART[0]);
		fprintf(fp, "Header: \r\n");
		buf_part = *(unsigned int*) ((char*)Frames[Nom_Fram] + sizeof(sFrame));
		printf("%2d ", ((buf_part >> 31) & 0x00000001)); // V (1)
		printf("%2d ", ((buf_part >> 30) & 0x00000001)); // P (1)
		printf("%2d ", ((buf_part >> 28) & 0x00000003)); // orb (2)
		printf("%5d ", ((buf_part >> 16) & 0x00000FFF)); // BCID (12)
		printf("\r\n");

		//************************************************************
		
		fprintf(fp,"%2d ", ((buf_part >> 31) & 0x00000001)); // V (1)
		fprintf(fp,"%2d ", ((buf_part >> 30) & 0x00000001)); // P (1)
		fprintf(fp,"%2d ", ((buf_part >> 28) & 0x00000003)); // orb (2)
		fprintf(fp,"%5d ", ((buf_part >> 16) & 0x00000FFF)); // BCID (12)
		fprintf(fp,"\r\n");


		//------------------------------------------------------------------------
		int i = 0;                                                 // Выводим данные 
		for (i = 0; i < FSizes[Nom_Fram] - 1; i++) {
			buf_part = *(unsigned int*)((char*)Frames[Nom_Fram] + sizeof(sFrame) + (i * 8 + 8));
			printf("%2d ", ((buf_part >> 31) & 0x00000001)); // 1 (1)
			printf("%2d ", ((buf_part >> 30) & 0x00000001)); // P (1)
			printf("%2d ", ((buf_part >> 29) & 0x00000001)); // R (1)
			printf("%5d ", ((buf_part >> 28) & 0x00000001)); // T (1)
			printf("%6d ", ((buf_part >> 22) & 0x0000003F)); // Chan# (6)
			printf("%7d ", ((buf_part >> 12) & 0x000003FF)); // ADC (10)
			printf("%4d ", ((buf_part >> 4) & 0x000000FF)); // TDC (8)
			printf("%2d ", ((buf_part >> 3) & 0x00000001)); // N (1)
			printf("%3d ", ((buf_part >> 0) & 0x00000007)); // rel BCID (3)
			printf("\r\n");

			//********************************************
			fprintf(fp,"%2d ", ((buf_part >> 31) & 0x00000001)); // 1 (1)
			fprintf(fp,"%2d ", ((buf_part >> 30) & 0x00000001)); // P (1)
			fprintf(fp,"%2d ", ((buf_part >> 29) & 0x00000001)); // R (1)
			fprintf(fp,"%5d ", ((buf_part >> 28) & 0x00000001)); // T (1)
			fprintf(fp,"%6d ", ((buf_part >> 22) & 0x0000003F)); // Chan# (6)
			fprintf(fp,"%7d ", ((buf_part >> 12) & 0x000003FF)); // ADC (10)
			fprintf(fp,"%4d ", ((buf_part >> 4) & 0x000000FF)); // TDC (8)
			fprintf(fp,"%2d ", ((buf_part >> 3) & 0x00000001)); // N (1)
			fprintf(fp,"%3d ", ((buf_part >> 0) & 0x00000007)); // rel BCID (3)
			fprintf(fp,"\r\n");

		}
	//	_getch();
	}


	// Frame[Номер Фрейма]->Поля фрейма  
	// Frames[0]->Frame_Number;// Номер фрейма 
	// Frames[0]->DDO[0];// DDO вские  
	// Frames[0]->ART;// ART

   // Запись в файл ==========================================================
	//f = fopen("test.txt","wb"); 
	//StrToFile(f,source);// Там немного хвостик образуеться 
	//=========================================================================
	fclose(fp);
	system("pause");
	return 0;
}
