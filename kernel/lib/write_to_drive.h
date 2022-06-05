enum drive { drive_first, drive_second, drive_third, drive_fourth };
enum access_mode { R, W };
enum origin { SEEK_SET, SEEK_CUR, SEEK_END };
struct drive_image {
    enum access_mode access_mode;
    enum drive drive;
    uint64_t byte_pointer_position;
};
struct drive_image* fopen(enum drive drive, enum access_mode access_mode);
uint8_t fseek(struct drive_image* drive_image, int64_t offset, enum origin origin);
void rewind(struct drive_image* drive_image);
void fclose(struct drive_image* drive_image);
uint64_t ftell(struct drive_image* drive_image);
uint8_t fwrite(uint8_t* ptr, size_t size_of_element, uint8_t number_of_elements,
               struct drive_image* image);
uint8_t fputs(char* str, struct drive_image* image);
uint8_t fflush(struct drive_image* image);
uint8_t fgetc(struct drive_image* image);
