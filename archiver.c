// Nume: Matisan Razvan-Andrei
// Grupa: 314CA

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NMAX 512
#define RECSIZE 512
#define NAMSIZ 100
#define OWN 32
#define DIM 12
#define NUM 8

// Functie care returneaza numarul de cuvinte delimitate de spatii.
int counter_words(char line[]) {
    int counter_words = 0;
    char *copy;
    copy = strtok(line, " \n");
    while (copy) {
        counter_words++;
        copy = strtok(NULL, " \n");
    }
    return counter_words;
}

// Functie care verifica valabilitatea comenzii "Create".
int check_create(char command[], char arch_name[], char arch_dir[]) {
    char copy_command[NMAX];
    strcpy(copy_command, command);
    int nr_words = counter_words(copy_command);
    if (nr_words == 3) {
        char *copy_word_command;
        copy_word_command = strtok(command, " \n");
        copy_word_command = strtok(NULL, " \n");
        strcpy(arch_name, copy_word_command);
        copy_word_command = strtok(NULL, " \n");
        strcpy(arch_dir, copy_word_command);
        long length_copy = strlen(copy_word_command);
        if (copy_word_command[length_copy - 1] == '/')
            return 1;
    }
    return 0;
}

union record {
    char charptr[RECSIZE];
    struct header {
        char name[NAMSIZ];
        char mode[NUM];
        char uid[NUM];
        char gid[NUM];
        char size[DIM];
        char mtime[DIM];
        char chksum[NUM];
        char typeflag;
        char linkname[NAMSIZ];
        char magic[NUM];
        char uname[OWN];
        char gname[OWN];
        char devmajor[NUM];
        char devminor[NUM];
    } header;
};

void complete_mode(union record *archive, char *word, char copy_word[NMAX]) {
    strcpy(archive->header.mode, "0000");
    memmove(copy_word, word + 1, NUM + 1);
    int nr_permissons = 0;
    while (nr_permissons < 3) {
        nr_permissons++;
        int code = 0;
        if (copy_word[0] == 'r')
            code += 4;
        if (copy_word[1] == 'w')
            code += 2;
        if (copy_word[2] == 'x')
            code += 1;
        char code_string[2];
        sprintf(code_string, "%d", code);
        strcat(archive->header.mode, code_string);
        memmove(copy_word, copy_word + 3, strlen(copy_word) - 3);
    }
}

long conversion_in_octal(long decimal) {
    long octal = 0, pow = 1;
    while (decimal) {
        octal += (decimal % 8) * pow;
        decimal /= 8;
        pow *= 10;
    }
    return octal;
}

long conversion_in_decimal(long octal) {
    long decimal = 0, pow = 1;
    while (octal) {
        decimal += (octal % 10) * pow;
        octal /= 10;
        pow *= 8;
    }
    return decimal;
}

int nr_digit(long n) {
    int nr = 0;
    if (n == 0) {
        return 1;
    } else {
        while (n) {
            n /= 10;
            nr++;
        }
    }
    return nr;
}

// Functie care imi adauga la inceputul unui buffer dat nr_zeros de "0".
void concat_zeros(char buffer[], int nr_zeros) {
    strcpy(buffer, "0");
    for (int i = 1; i < nr_zeros; i++) {
        strcat(buffer, "0");
    }
}

// Functie care imi adauga la finalul unui buffer dat n de "\0".
void concat_bzeros(int n, char buffer[]) {
    for (int i = strlen(buffer); i < n; i++) {
        buffer[i] = '\0';
    }
}

unsigned long checksum(union record archive) {
    // inital, archive.header.chksum are 8 spatii, iar un spatiu in ASCII = 32.
    unsigned long sum = NUM * 32;
    unsigned int i;
    for (i = 0; i < strlen(archive.header.name); i++) {
        sum += archive.header.name[i];
        sum += archive.header.linkname[i];
    }
    for (i = 0; i < strlen(archive.header.mode); i++)
        sum += archive.header.mode[i];
    for (i = 0; i < strlen(archive.header.uid); i++)
        sum += archive.header.uid[i];
    for (i = 0; i < strlen(archive.header.gid); i++)
        sum += archive.header.gid[i];
    for (i = 0; i < strlen(archive.header.size); i++)
        sum += archive.header.size[i];
    for (i = 0; i < strlen(archive.header.mtime); i++)
        sum += archive.header.mtime[i];
    for (i = 0; i < strlen(archive.header.magic); i++)
        sum += archive.header.magic[i];
    for (i = 0; i < strlen(archive.header.uname); i++)
        sum += archive.header.uname[i];
    for (i = 0; i < strlen(archive.header.gname); i++)
        sum += archive.header.gname[i];
    return sum;
}

// Functie care imi copiaza ora fara milisecunde Ifara ceea ce se afla dupa .
// in files.txt.
void copy_hour(char hour[], char copy_word[]) {
    int c = 0;
    while (copy_word[c] != '.') {
        hour[c] = copy_word[c];
        c++;
    }
}

void read_files(union record *archive, char line_files[]) {
    strcpy(archive->header.magic, "GNUtar ");
    char *word, copy_word[NMAX], date[NMAX];
    word = strtok(line_files, " \n");
    int counter_files = 0;
    archive->header.typeflag = '\0';
    memset(archive->header.devmajor, 0, sizeof(archive->header.devmajor));
    memset(archive->header.devminor, 0, sizeof(archive->header.devmajor));
    while (word) {
        counter_files++;
        memset(copy_word, 0, sizeof(copy_word));
        if (counter_files == 1)
            complete_mode(archive, word, copy_word);
        if (counter_files == 3) {
            memset(archive->header.uname, 0, sizeof(archive->header.uname));
            strcpy(archive->header.uname, word);
            concat_bzeros(OWN, archive->header.uname);
        }
        if (counter_files == 4) {
            memset(archive->header.gname, 0, sizeof(archive->header.gname));
            strcpy(archive->header.gname, word);
            concat_bzeros(OWN, archive->header.gname);
        }
        if (counter_files == 5) {
            char copy_octal_buffer[DIM];
            memset(archive->header.size, 0, sizeof(archive->header.size));
            memset(copy_octal_buffer, 0, sizeof(copy_octal_buffer));
            strcpy(copy_word, word);
            int decimal;
            sscanf(copy_word, "%d", &decimal);
            int octal = conversion_in_octal(decimal);
            int copy_octal = octal, nr_digits = nr_digit(copy_octal);
            concat_zeros(archive->header.size, DIM - 1 - nr_digits);
            sprintf(copy_octal_buffer, "%d", octal);
            strcat(archive->header.size, copy_octal_buffer);
        }
        if (counter_files == 6) {
            strcpy(date, word);
            strcat(date, " ");
        }
        if (counter_files == 7) {
            memset(archive->header.mtime, 0, sizeof(archive->header.mtime));
            strcpy(copy_word, word);
            char hour[DIM];
            memset(hour, 0, sizeof(hour));
            copy_hour(hour, copy_word);
            strcat(date, hour);
            struct tm time;
            memset(&time, 0, sizeof(time));
            strptime(date, "%Y-%m-%d %H:%M:%S", &time);
            long octal_time = conversion_in_octal(mktime(&time));
            sprintf(archive->header.mtime, "%ld", octal_time);
        }
        if (counter_files == 9) {
            strcpy(archive->header.name, word);
            strcpy(archive->header.linkname, word);
            concat_bzeros(NAMSIZ, archive->header.name);
            concat_bzeros(NAMSIZ, archive->header.linkname);
        }
        word = strtok(NULL, " \n");
    }
}

void read_usermap(union record *archive) {
    FILE *usermap_in = fopen("usermap.txt", "rt");
    char line_usermap[NMAX];
    while (fgets(line_usermap, NMAX, usermap_in)) {
        char *word2;
        word2 = strtok(line_usermap, ":\n");
        if (strcmp(word2, archive->header.uname) == 0) {
            int counter_usermap = 0;
            while (counter_usermap < 4) {
                counter_usermap++;
                if (counter_usermap == 3) {
                    concat_zeros(archive->header.uid, NUM - 1 - strlen(word2));
                    strcat(archive->header.uid, word2);
                }
                if (counter_usermap == 4) {
                    concat_zeros(archive->header.gid, NUM - 1 - strlen(word2));
                    strcat(archive->header.gid, word2);
                }
                word2 = strtok(NULL, ":\n");
            }
        }
    }
    fclose(usermap_in);
}

void solve_create(union record archive, char arch_name[], char arch_dir[]) {
    FILE *files_in = fopen("files.txt", "rt");
    FILE *archive_in = fopen(arch_name, "wb");
    FILE *usermap_in = fopen("usermap.txt", "rt");
    char line_files[NMAX];
    memset(line_files, 0, sizeof(line_files));
    while (fgets(line_files, NMAX, files_in)) {
        read_files(&archive, line_files);
        read_usermap(&archive);
        // Convertire chksum in octal + copierea lui in archive.header.chksum.
        long decimal_chksum = checksum(archive);
        long octal_chksum = conversion_in_octal(decimal_chksum);
        long octal_chksum2 = octal_chksum;
        concat_zeros(archive.header.chksum, NUM - 1 - nr_digit(octal_chksum2));
        char copy_octal_buffer[NMAX];
        sprintf(copy_octal_buffer, "%ld", octal_chksum);
        strcat(archive.header.chksum, copy_octal_buffer);
        // Copierea header-ului fisierului in arhiva creata.
        fwrite(&archive.header, 1, sizeof(archive.header), archive_in);
        // Completarea cu '\0' pana la 512 bytes.
        char buffer_zeros[RECSIZE - sizeof(archive.header)];
        memset(buffer_zeros, 0, sizeof(buffer_zeros));
        fwrite(buffer_zeros, 1, sizeof(buffer_zeros), archive_in);
        char copy_arch_dir[RECSIZE];
        strcpy(copy_arch_dir, arch_dir);
        strcat(copy_arch_dir, archive.header.name);
        // Copierea continutului fisierului in blocuri de 512 bytes.
        FILE *name_file = fopen(copy_arch_dir, "rb");
        char string[RECSIZE];
        if (name_file) {
            memset(string, 0, sizeof(string));
            while (fread(string, 1, sizeof(string), name_file)) {
                fwrite(string, 1, sizeof(string), archive_in);
                memset(string, 0, sizeof(string));
            }
            fclose(name_file);
        }
        memset(&archive.header, 0, sizeof(archive.header));
        memset(line_files, 0, sizeof(line_files));
    }
    // Completarea cu 512 de '\0' la finalul arhivei.
    char buffer_zeros2[RECSIZE];
    memset(buffer_zeros2, 0, sizeof(buffer_zeros2));
    fwrite(buffer_zeros2, 1, sizeof(buffer_zeros2), archive_in);
    if (!usermap_in || !archive_in || !files_in)
        printf("> Failed!\n");
    else
        printf("> Done!\n");
    fclose(archive_in);
    fclose(files_in);
    fclose(usermap_in);
}

void solve_list(char command[]) {
    char copy_command[NMAX], copy2_command[NMAX], name_file[NMAX];
    strcpy(copy_command, command);
    strcpy(copy2_command, command);
    int nr_words = counter_words(copy2_command);
    if (nr_words == 2) {
        char *copy_word_command;
        copy_word_command = strtok(copy_command, " \n");
        copy_word_command = strtok(NULL, " \n");
        strcpy(name_file, copy_word_command);
        FILE *archive_in = fopen(name_file, "rb");
        if (!archive_in) {
            printf("> File not found!\n");
        } else {
            char header[RECSIZE];
            while (fread(header, 1, sizeof(header), archive_in) && header[0]) {
                printf("> %s\n", header);
                char size[DIM];
                strcpy(header, header + NAMSIZ + 3 * NUM);
                memset(size, 0, sizeof(size));
                strncat(size, header, DIM);
                long octal;
                sscanf(size, "%ld", &octal);
                long decimal = conversion_in_decimal(octal);
                char content[RECSIZE];
                int nr_blocks = (decimal + RECSIZE - 1) / RECSIZE;
                // Se vor citi nr_blocks buffer-uri de 512 bytes pentru a
                // muta cursorul dupa continutul fisierului
                // (inaintea urmatorului header).
                for (int i = 0; i < nr_blocks; i++)
                    fread(content, 1, sizeof(content), archive_in);
                memset(header, 0, sizeof(header));
            }
            fclose(archive_in);
        }
    } else {
        printf("> Wrong command!\n");
    }
}

void solve_extract(char command[]) {
    char copy_command[NMAX], copy2_command[NMAX];
    char name_archive[NMAX], extracted_file[NAMSIZ];
    memset(extracted_file, 0, sizeof(extracted_file));
    strcpy(copy_command, command);
    strcpy(copy2_command, command);
    int nr_words = counter_words(copy2_command);
    if (nr_words == 3) {
        char *copy_word_command;
        copy_word_command = strtok(copy_command, " \n");
        copy_word_command = strtok(NULL, " \n");
        strcpy(extracted_file, copy_word_command);
        copy_word_command = strtok(NULL, " \n");
        strcpy(name_archive, copy_word_command);
        FILE *archive_in = fopen(name_archive, "rb");
        if (!archive_in) {
            printf("> File not found!\n");
        } else {
            char header[RECSIZE];
            int ok = 0;
            while (fread(header, 1, sizeof(header), archive_in) && header[0]) {
                char name_file[NAMSIZ];
                memset(name_file, 0, sizeof(name_file));
                strncat(name_file, header, NAMSIZ);
                char size[DIM];
                strcpy(header, header + NAMSIZ + 3 * NUM);
                memset(size, 0, sizeof(size));
                strncat(size, header, DIM);
                long octal;
                sscanf(size, "%ld", &octal);
                long decimal = conversion_in_decimal(octal);
                char content[RECSIZE];
                // Se verifica daca numele fisierului din arhiva coincide
                // cu cel din comanda.
                if (strcmp(name_file, extracted_file) == 0) {
                    ok = 1;
                    char extracted[NAMSIZ];
                    memset(extracted, 0, sizeof(extracted));
                    strcat(extracted, "extracted_");
                    strcat(extracted, extracted_file);
                    FILE *extract = fopen(extracted, "wb");
                    int nr_blocks = (decimal + RECSIZE - 1) / RECSIZE;
                    // Comparativ cu list, aici continutul este copiat in
                    // fisierul extras.
                    for (int i = 0; i < nr_blocks; i++) {
                        if (i == nr_blocks - 1) {
                            fread(content, 1, sizeof(content), archive_in);
                            if (decimal % RECSIZE)
                                fwrite(content, 1, decimal % RECSIZE, extract);
                            else
                                fwrite(content, 1, sizeof(content), extract);
                        } else {
                            fread(content, 1, sizeof(content), archive_in);
                            fwrite(content, 1, sizeof(content), extract);
                        }
                    }
                    memset(header, 0, sizeof(header));
                    printf("> File extracted!\n");
                    fclose(extract);
                } else {
                    int nr_blocks = (decimal + RECSIZE - 1) / RECSIZE;
                    for (int i = 0; i < nr_blocks; i++)
                        fread(content, 1, sizeof(content), archive_in);
                }
            }
            if (!ok)
                printf("> File not found!\n");
            fclose(archive_in);
        }
    } else {
        printf("> Wrong command!\n");
    }
}

int main() {
    int command_exit = 0;
    while (!command_exit) {
        char command[NMAX], copy_command[NMAX];
        fgets(command, NMAX, stdin);
        strcpy(copy_command, command);
        char *copy_word_command = strtok(copy_command, " \n");
        if (!copy_word_command) {
            printf("> Wrong command!\n");
        } else if (strcmp(command, "exit\n") == 0) {
            command_exit = 1;
        } else if (strcmp(copy_word_command, "create") == 0) {
            memset(copy_command, 0, strlen(copy_command));
            strcpy(copy_command, command);
            char arch_name[NMAX], arch_dir[NMAX];
            if (check_create(copy_command, arch_name, arch_dir) == 1) {
                union record archive;
                solve_create(archive, arch_name, arch_dir);
            } else {
                printf("> Wrong command!\n");
            }
        } else if (strcmp(copy_word_command, "list") == 0) {
            memset(copy_command, 0, strlen(copy_command));
            strcpy(copy_command, command);
            solve_list(copy_command);
        } else if (strcmp(copy_word_command, "extract") == 0) {
            memset(copy_command, 0, strlen(copy_command));
            strcpy(copy_command, command);
            solve_extract(copy_command);
        } else {
            printf("> Wrong command!\n");
        }
        memset(command, 0, sizeof(command));
        memset(copy_command, 0, sizeof(copy_command));
    }
}
