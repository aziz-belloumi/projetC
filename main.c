#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILENAME "hospital_records.txt"

// Structure to store patient information
typedef struct {
    char id[10];
    char name[50];
    int age;
    char disease[50];
} Patient;

// Function prototypes
void createRecord();
void modifyRecord();
void deleteRecord();
void displayRecords();
void backupFile();
void logAction(const char *action);
void searchByName();
void sortRecordsByAge();

int main() {
    int choice;

    while (1) {
        printf("\nHospital File Management System\n");
        printf("1. Create Record\n");
        printf("2. Modify Record\n");
        printf("3. Delete Record\n");
        printf("4. Display Records\n");
        printf("5. Search by Name\n");
        printf("6. Sort Records by Age\n");
        printf("7. Backup File\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // To consume the newline character

        switch (choice) {
            case 1:
                createRecord();
                break;
            case 2:
                modifyRecord();
                break;
            case 3:
                deleteRecord();
                break;
            case 4:
                displayRecords();
                break;
            case 5:
                searchByName();
                break;
            case 6:
                sortRecordsByAge();
                break;
            case 7:
                backupFile();
                break;
            case 8:
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}

// Function to create a new record
void createRecord() {
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    Patient p;
    printf("Enter Patient ID: ");
    scanf("%s", p.id);
    printf("Enter Patient Name: ");
    scanf(" %[^\n]", p.name); // Allows spaces in input
    printf("Enter Age: ");
    scanf("%d", &p.age);
    printf("Enter Disease: ");
    scanf(" %[^\n]", p.disease);

    fwrite(&p, sizeof(Patient), 1, file);
    fclose(file);
    logAction("Created new record.");
    printf("Record added successfully.\n");
}

// Function to modify an existing record
void modifyRecord() {
    FILE *file = fopen(FILENAME, "r+");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    char id[10];
    int found = 0;
    Patient p;

    printf("Enter Patient ID to modify: ");
    scanf("%s", id);

    while (fread(&p, sizeof(Patient), 1, file)) {
        if (strcmp(p.id, id) == 0) {
            found = 1;
            printf("Enter new Patient Name: ");
            scanf(" %[^\n]", p.name);
            printf("Enter new Age: ");
            scanf("%d", &p.age);
            printf("Enter new Disease: ");
            scanf(" %[^\n]", p.disease);

            fseek(file, -sizeof(Patient), SEEK_CUR);
            fwrite(&p, sizeof(Patient), 1, file);
            logAction("Modified an existing record.");
            printf("Record updated successfully.\n");
            break;
        }
    }

    if (!found) {
        printf("Record not found.\n");
    }

    fclose(file);
}

// Function to delete a record
void deleteRecord() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    FILE *tempFile = fopen("temp.txt", "w");
    if (!tempFile) {
        printf("Error creating temporary file!\n");
        fclose(file);
        return;
    }

    char id[10];
    int found = 0;
    Patient p;

    printf("Enter Patient ID to delete: ");
    scanf("%s", id);

    while (fread(&p, sizeof(Patient), 1, file)) {
        if (strcmp(p.id, id) != 0) {
            fwrite(&p, sizeof(Patient), 1, tempFile);
        } else {
            found = 1;
        }
    }

    fclose(file);
    fclose(tempFile);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found) {
        logAction("Deleted a record.");
        printf("Record deleted successfully.\n");
    } else {
        printf("Record not found.\n");
    }
}

// Function to display all records
void displayRecords() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("No records found!\n");
        return;
    }

    Patient p;
    printf("\nPatient Records:\n");
    printf("ID\tName\t\tAge\tDisease\n");
    printf("---------------------------------------------\n");

    while (fread(&p, sizeof(Patient), 1, file)) {
        printf("%s\t%s\t\t%d\t%s\n", p.id, p.name, p.age, p.disease);
    }

    fclose(file);
}

// Function to backup the file
void backupFile() {
    FILE *source = fopen(FILENAME, "r");
    FILE *destination = fopen("hospital_backup.txt", "w");

    if (!source || !destination) {
        printf("Error in creating backup file.\n");
        return;
    }

    char ch;
    while ((ch = fgetc(source)) != EOF) {
        fputc(ch, destination);
    }

    fclose(source);
    fclose(destination);
    logAction("Backup created.");
    printf("Backup created successfully.\n");
}

// Function to log actions
void logAction(const char *action) {
    FILE *logFile = fopen("log.txt", "a");
    if (logFile) {
        time_t now = time(NULL);
        fprintf(logFile, "%s - %s\n", ctime(&now), action);
        fclose(logFile);
    }
}

// Function to search records by patient name
void searchByName() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("No records found!\n");
        return;
    }

    char name[50];
    Patient p;
    printf("Enter Patient Name to search: ");
    scanf(" %[^\n]", name);

    int found = 0;
    while (fread(&p, sizeof(Patient), 1, file)) {
        if (strstr(p.name, name)) {
            printf("%s\t%s\t\t%d\t%s\n", p.id, p.name, p.age, p.disease);
            found = 1;
        }
    }

    if (!found) {
        printf("No records found for this name.\n");
    }

    fclose(file);
}

// Function to sort records by age
void sortRecordsByAge() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("No records found!\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    int count = size / sizeof(Patient);

    fseek(file, 0, SEEK_SET);
    Patient *records = (Patient *)malloc(size);
    fread(records, sizeof(Patient), count, file);
    fclose(file);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (records[i].age > records[j].age) {
                Patient temp = records[i];
                records[i] = records[j];
                records[j] = temp;
            }
        }
    }

    printf("\nSorted Records by Age:\n");
    for (int i = 0; i < count; i++) {
        printf("%s\t%s\t\t%d\t%s\n", records[i].id, records[i].name, records[i].age, records[i].disease);
    }

    free(records);
}
