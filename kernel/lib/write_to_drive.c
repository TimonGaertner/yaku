#include "write_to_drive.h"
#include <memory/pmm.h>

struct drive_image* fopen(enum drive drive, enum access_mode access_mode){
    if (drive==drive_first){
        if (!drive_present(primary_controller, first_drive)){
            return NULL;
        }
    } else if (drive==drive_second){
        if (!drive_present(primary_controller, second_drive)){
            return NULL;
        }
    } else if (drive==drive_third){
        if (!drive_present(secondary_controller, first_drive)){
            return NULL;
        }
    } else if (drive==drive_fourth){
        if (!drive_present(secondary_controller, second_drive)){
            return NULL;
        }
    }
    struct drive_image* drive_image = (struct drive_image) malloc(sizeof(struct drive_image)/4069+1);
    drive_image->drive=drive;
    drive_image->access_mode=access_mode;
    drive_image->sector_pointer=0;
    return &drive_image;
}
uint64_t ftell(struct drive_image*){
    return drive_image->sector_pointer;
}
void fclose(struct drive_image*){
    free(drive_image, sizeof(drive_image)/4069+1);
}
void rewind(struct drive_image*){
    drive_image->drive=0;    
}
uint16_t fseek(struct drive_image*, uint64_t offset, enum origin){
    if (origin==SEEK_SET){
        drive_image->sector_pointer=offset;
        return 0;
    } else if (origin==SEEK_CUR){
        drive_image->sector_pointer+=offset;
        return 0:
    } else if (origin=SEEK_END){
        if (drive_image->drive==drive_first){
            if (!drive_present(primary_controller, first_drive)){
            return 1;
        }
        drive_image->sector_pointer=get_master_drive_size(primary_controller, first_drive)+offset;
        return 0;
    } else if (drive==drive_second){
        if (!drive_present(primary_controller, second_drive)){
            return 1;
        }
        drive_image->sector_pointer=get_master_drive_size(primary_controller, second_drive)+offset;
        return 0;
    } else if (drive_image->drive==drive_third){
        if (!drive_present(secondary_controller, first_drive)){
            return 1;
        }
        drive_image->sector_pointer=get_master_drive_size(secondary_controller, first_drive)+offset;
        return 0;
    } else if (drive_image->drive==drive_fourth){
        if (!drive_present(secondary_controller, second_drive)){
            return 1;
        }
        drive_image->sector_pointer=get_master_drive_size(secondary_controller, second_drive)+offset;
        return 0;
    }
        
    }
}
