#include <stdio.h>

// print one char
void printChar(unsigned char c)
{
    printf("0x%02x=", (int)c);
    if (c == 0) {
        printf("<null>");
    }
    else if (c >= 32 && c <= 126) {
        printf("'%c'", c);
    } else {
        printf("<unprintable>");
    }
}

void dumpNext(char *filename, FILE *fp, unsigned char c)
{
    //The next 4 bytes of 'main.c' are:0x0a=<unprintable> 0x76='v' 0x6f='o' 0x69='i'
    int inx = ftell(fp);
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    // how many char left
    int left = size - inx + 1;
    int i = 0;
    fseek(fp, inx, SEEK_SET);
    if (left < 4) {
        printf("'%s' only has %d bytes left:", filename, left);
    } else {
        printf("The next 4 bytes of '%s' are:", filename);
        left = 4;
    }
    // output the different char
    for (i = 0; i < left; i++) {
        printChar(c);
        if (i == left - 1) {
            printf("\n");
            break;
        } else {
            printf(" ");
            c = fgetc(fp);
        }
    }
}

int main(int argc, char *argv[]) {
    FILE *fp1, *fp2;
    int c1, c2;
    int found = 0;
    int inx = 0;
    if (argc != 3) {
        fprintf(stderr, "SYNTAX: %s <file1> <file2>\n", argv[0]);
        return 1;
    }
    fp1 = fopen(argv[1], "r");
    if (fp1 == NULL) {
        fprintf(stderr, "Cannot open first file: No such file or directory\n");
        return 1;
    }
    fp2 = fopen(argv[2], "r");
    if (fp2 == NULL) {
        fprintf(stderr, "Cannot open second file: No such file or directory\n");
        fclose(fp1);
        return 1;
    }

    while (!feof(fp1) || !feof(fp2)) {
        // get next char
        c1 = fgetc(fp1);
        c2 = fgetc(fp2);
        // if file 2 is end
        if (c1 == -1 && c2 != -1) {
            printf("The file '%s' was shorter than '%s'.\n", argv[1], argv[2]);
            dumpNext(argv[2], fp2, c2);
            found = 1;
            break;
        } else if (c1 != -1 && c2 == -1) {
        // if file1 is end
            printf("The file '%s' was shorter than '%s'.\n", argv[2], argv[1]);
            dumpNext(argv[1], fp1, c1);
            found = 1;
            break;
        } else if (c1 == -1 && c2 == -1) {
        // if files are end
            break;
        } else if (c1 != c2) {
        // if diff found
            printf("Difference found at byte %d.\n", inx);
            dumpNext(argv[1], fp1, c1);
            dumpNext(argv[2], fp2, c2);
            found = 1;
            break;
        }
        inx++;
    }

    // close files
    fclose(fp1);
    fclose(fp2);

    if (found == 0) {
        printf("No differences found.\n");
    }

    return found;
}
