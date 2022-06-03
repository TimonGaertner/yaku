enum drive{
    drive_first,
    drive_second,
    drive_third,
    drive_fourth
}
enum access_mode{
    R,
    W
}
enum sector_pointer_position{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
}
struct drive_image {
    enum access_mode access_mode;
    enum drive drive;
    uint64_t sector_pointer_position;
}
enum drive fopen(enum drive drive, enum access_mode access_mode);
