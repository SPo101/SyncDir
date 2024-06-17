#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>//for scandir

int Usage(){
	printf("Usage: dir_sync <count of dirrectories to sync>\n");
	printf("path to dir 1\n...\npath to dir n\n");
	exit(0);
}

struct Pathi{
	char* path;
	struct Pathi* next;
};

struct Dir{
	char* path;
	char **files;
};

char* input_from_user(){
	char buff[512];
	int i=0;
	while(1){
		read(0, &buff[i], 1);
		if(buff[i] == '\n')
			break;
		i++;
	}

	char* text = (char*) malloc(i+1);
	memcpy(text, &buff, i);
	return text;
}

void prints(char* text){
	for(int i=0; i<strlen(text); i++)
		printf("%c", *(text+i));
}

struct Pathi* addPath(struct Pathi* root, char* path){
	struct Pathi* newPath = malloc(sizeof(struct Pathi));
	newPath->path = path;
	if(root != NULL)
		root->next = newPath;
	return newPath;
}

char* concs(char* s1, char* s2){
	char* text = (char*) malloc(strlen(s1) + strlen(s2));
	memcpy(text, s1, strlen(s1));
	memcpy(text+strlen(s1), s2, strlen(s2));
	return text;
}

void syncfiles(char** files, char** sfiles, char* path){
	struct dirent **namelist;
	int n;
	int offset=0;
	while(*(files+offset)!=NULL)
		offset++;

	n = scandir(path, &namelist, NULL, alphasort);
	for(int i=0; i<n; i++){
		sfiles[i+offset] = "";
		files[i+offset] = (char*) malloc(sizeof(namelist[i]->d_name));
			int ok = 1;
			for(int j=0; j<i+offset; j++){
				if(strcmp(files[j], namelist[i]->d_name) == 0){
					ok = 0;
					break;
				}	
			}
			if(ok){
				char ch = '/';
				sfiles[i+offset] = (char*) malloc(sizeof(namelist[i]->d_name)+strlen(path)+1);
				memcpy(sfiles[i+offset], path, strlen(path));
				memcpy(sfiles[i+offset]+strlen(path), &ch, 1);
				memcpy(sfiles[i+offset]+strlen(path)+1, namelist[i]->d_name, strlen(namelist[i]->d_name));
				memcpy(files[i+offset], namelist[i]->d_name, strlen(namelist[i]->d_name));
			}
		free(namelist[i]);
	}
	free(namelist);
}

void showdir(char* path){
	struct dirent **namelist;
	int n;

	n = scandir(path, &namelist, NULL, alphasort);
	while(n--){
		printf("%s ", namelist[n]->d_name);
		fflush(stdout);
		free(namelist[n]);
	}
	free(namelist);
	printf("\n");
}

int main(int argc, char* argv[]){
	if(argc < 2)
		Usage();


	struct Pathi* root = NULL;
	struct Pathi* current = NULL;
	char** files;
	char** sfiles;

	int cnt = atoi(argv[1]);
	for(int i=0; i<cnt; i++){
		char* path = input_from_user();
		while(*(path+strlen(path)-1) == '/'){
			showdir(path);
			prints(path);
			fflush(stdout);
			path = concs(path, input_from_user());
		}
		current = addPath(current, path);
		if(i==0)
			root = current;
	}



	current = root;
	files = malloc(sizeof(char*)*128);
	sfiles = malloc(sizeof(char*)*128);
	printf("\n\n");
	for(int i=0; i<cnt; i++){
		//prints((current->path)); printf("\n");
		//showdir(current->path); printf("\n");
		syncfiles(files, sfiles, current->path);
		current = current->next;
	}

	int i=0;
	while(*(sfiles+i)!=NULL){
		if( *(sfiles+i)[0] == '.'){
			prints(*(sfiles+i)); printf("\n");
		}
		i++;
	}
	return 0;
}
