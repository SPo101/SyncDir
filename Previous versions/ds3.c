#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>//for scandir
#include <sys/types.h>
#include <sys/stat.h>

int Usage(){
	printf("Usage: dir_sync <count of dirrectories to sync>\n");
	printf("path to dir 1\n...\npath to dir n\n");
	exit(0);
}

struct Dir{
	int count;
	char* path;
	struct dirent **files;
	struct Dir* next;
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

void printd(struct Dir* current){
	for(int i=0; i<current->count; i++)
		printf("%s ", current->files[i]->d_name);
}

struct Dir* addDir(struct Dir* root, char* path){
	struct Dir* newDir = malloc(sizeof(struct Dir));
	newDir->path = path;

	newDir->count = scandir(path, &newDir->files, NULL, alphasort);
	if(root != NULL)
		root->next = newDir;
	return newDir;
}

char* concs(char* s1, char* s2){
	char* text = (char*) malloc(strlen(s1) + strlen(s2));
	memcpy(text, s1, strlen(s1));
	memcpy(text+strlen(s1), s2, strlen(s2));
	return text;
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

int findfile(char* fname, struct Dir* path){
	int ok = 0;
	for(int i=0; i<path->count; i++){
		int res = strcmp(fname, path->files[i]->d_name);
		if(res == 0)
			ok = 1;
	}
	return ok;
}

int isDir(const char* fileName)
{
    struct stat path;
    stat(fileName, &path);
    return S_ISREG(path.st_mode); //0-dir
}

void copy_from_one_to_all(struct Dir* path, struct Dir* root){
	for(int i=0; i<path->count; i++){
		char* fname = path->files[i]->d_name;
		
		struct Dir* current = root;
		while(current != NULL){
			if( findfile(fname, current) == 0){
				//shit code on
				int a1 = strlen(path->path);
				int a2 = strlen(path->files[i]->d_name);
				int a3 = strlen(current->path);
				int lencom = 5+ a1 + a2 + a3;				
				char *com = (char*) malloc(lencom);
				char *file = (char*) malloc(1+a1+a2);
				strcpy(file, path->path);
				strcpy(file+a1, "/");
				strcpy(file+1+a1, path->files[i]->d_name);
				if( isDir(file) == 0){
				strcpy(com, "cp -r ");
				strcpy(com+6, path->path);
				strcpy(com+6+a1, "/");
				strcpy(com+7+a1, path->files[i]->d_name);
				strcpy(com+7+a1+a2, " ");
				strcpy(com+8+a1+a2, current->path);
				}
				else{
				strcpy(com, "cp ");
				strcpy(com+3, path->path);
				strcpy(com+3+a1, "/");
				strcpy(com+4+a1, path->files[i]->d_name);
				strcpy(com+4+a1+a2, " ");
				strcpy(com+5+a1+a2, current->path);
				}
				
				system(com);
				//shit code off
			}
			current = current->next;
		}
	}
}

int main(int argc, char* argv[]){
	if(argc < 2)
		Usage();


	struct Dir* root = NULL;
	struct Dir* current = NULL;

	int cnt = atoi(argv[1]);
	int fcnt=0;
	for(int i=0; i<cnt; i++){
		char* path = input_from_user();
		while(*(path+strlen(path)-1) == '/'){
			showdir(path);
			prints(path);
			fflush(stdout);
			path = concs(path, input_from_user());
		}
		current = addDir(current, path);
		fcnt+=current->count;
		if(i==0)
			root = current;
	}

	current = root;
	while(current != NULL){
		copy_from_one_to_all(current, root);
		current = current->next;
	}

	printf("\nDONE!");
	return 0;
}


