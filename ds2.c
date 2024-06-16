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
};

struct Pathi{
	struct Dir* dir;
	struct Pathi* next;
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

void printd(struct Pathi* current){
	for(int i=0; i<current->dir->count; i++)
		printf("%s ", current->dir->files[i]->d_name);
}

struct Pathi* addPath(struct Pathi* root, char* path){
	struct Pathi* newPath = malloc(sizeof(struct Pathi));
	struct Dir* newDir = malloc(sizeof(struct Dir));
	newPath->dir = newDir;
	newDir->path = path;

	newDir->count = scandir(path, &newDir->files, NULL, alphasort);
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

int findfile(char* fname, struct Pathi* path){
	int ok = 0;
	for(int i=0; i<path->dir->count; i++){
		int res = strcmp(fname, path->dir->files[i]->d_name);
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

void copy_from_one_to_all(struct Pathi* path, struct Pathi* root){
	for(int i=0; i<path->dir->count; i++){
		char* fname = path->dir->files[i]->d_name;
		
		struct Pathi* current = root;
		while(current != NULL){
			if( findfile(fname, current) == 0){
				//shit code on
				int a1 = strlen(path->dir->path);
				int a2 = strlen(path->dir->files[i]->d_name);
				int a3 = strlen(current->dir->path);
				int lencom = 5+ a1 + a2 + a3;				
				char *com = (char*) malloc(lencom);
				char *file = (char*) malloc(1+a1+a2);
				strcpy(file, path->dir->path);
				strcpy(file+a1, "/");
				strcpy(file+1+a1, path->dir->files[i]->d_name);
				if( isDir(file) == 0){
				strcpy(com, "cp -r ");
				strcpy(com+6, path->dir->path);
				strcpy(com+6+a1, "/");
				strcpy(com+7+a1, path->dir->files[i]->d_name);
				strcpy(com+7+a1+a2, " ");
				strcpy(com+8+a1+a2, current->dir->path);
				}
				else{
				strcpy(com, "cp ");
				strcpy(com+3, path->dir->path);
				strcpy(com+3+a1, "/");
				strcpy(com+4+a1, path->dir->files[i]->d_name);
				strcpy(com+4+a1+a2, " ");
				strcpy(com+5+a1+a2, current->dir->path);
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


	struct Pathi* root = NULL;
	struct Pathi* current = NULL;

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
		current = addPath(current, path);
		fcnt+=current->dir->count;
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


