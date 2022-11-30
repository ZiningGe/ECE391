#include "filesystem.h"
#include "lib.h"
#include "terminal.h"


//variable list 
//variables for file read
dir_entry new_entry;
uint32_t cur_offest;
static boot_block* bootblock_start = NULL;
uint8_t dir_idx = 0;



boot_block* getboot_block() {
    return bootblock_start;
}


/* fs_init
 * Description: initialize the bootblock
 * Inputs:  boot_block_addr -- the address of the bootblock
 * Return Value:0 if success -1 if not
 * Function: store the bootblock to bootblock_start
 */
int32_t fs_init(uint32_t boot_block_addr){
    bootblock_start = (boot_block*)boot_block_addr;
    return 0;
}


/* fs_open
 * Description: open the file
 * Inputs:  filename
 * Return Value:0 if success -1 if not
 * Function: store the dentry to new_entry
 */
int32_t fs_open(const int8_t *filename){
    // If the named file does not exist or no descriptors are free, the call returns -1.
    int retval;
    // sanity check
    if (!filename | !bootblock_start) {
        return -1;
    }

    // read dentry
    retval = read_dentry_by_name((int8_t*)filename, &new_entry);


    //check type of the file
    if (retval != 0 || new_entry.file_type != REGULAR){
        return -1;
    }
    return 0;
}


/* fs_close
 * Description: close the file
 * Inputs:  fd
 * Return Value:0 if success -1 if not
 * Function: do nothing
 */
int32_t fs_close (int32_t fd) {
    return 0;
}


/* fs_read
 * Description: read the data in the file
 * Inputs:  fd
 *          buf: buffer to store the chars
 *          nbyte: length of the buf
 * Return Value:0 if success -1 if not
 * Function: read date into buffer
 */
int32_t fs_read(int32_t fd, void* buf, int32_t nbytes){
    uint32_t inode_index = curr_PCB[curr_rid]->fd_arr[fd].inode;
    uint32_t cur_read_offest = curr_PCB[curr_rid]->fd_arr[fd].file_position;
    //read data from datablock
    return read_data(inode_index, cur_read_offest, buf, nbytes);
}

/* fs_write
 * Description: do nothing
 * Inputs:  fd
 *          buf: buffer to store the chars
 *          nbyte: length of the buf
 * Return Value:0 if success -1 if not
 * Function: do nothing
 */
int32_t fs_write (int32_t fd, const void* buf, int32_t nbytes){
    return -1; // read only
}

/* dir_open
 * Description: open directory
 * Inputs:  filename: the file to be opened
 * Return Value:0 if success -1 if not
 * Function: initialize the sequence of read dir
 */
int32_t dir_open(const int8_t *filename){
     if (!filename | !bootblock_start) { //check if bootblock or filename is null
        return -1;
    }
    
    dir_entry cur_entry;
    int8_t* fn = (int8_t*)filename;
    // if(read_dentry_by_name(filename,&cur_entry) == -1) return -1;
    // if(cur_entry.file_type != 1) return -1;
    // return cur_entry.inode_num;
    return read_dentry_by_name(fn,&cur_entry);
}

/* dir_close
 * Description: do nothing
 * Inputs:  fd
 * Return Value:0 if success -1 if not
 * Function: do nothing
 */
int32_t dir_close (int32_t fd){
    return 0;
}


/* dir_read
 * Description: read the directory by sequence
 * Inputs:  fd
 *          buf: buffer to store the chars
 *          nbyte: length of the buf
 * Return Value:0 if success -1 if not
 * Function: read each directory
 */
int32_t dir_read(int32_t fd, void * buf, int32_t nbytes){
    // TODO
    if (bootblock_start == NULL) {
        return -1; //Null checking 
    }
    if (buf == NULL) {
        return -1; // Null Check Buffer
    }
    if (curr_PCB[curr_rid]->fd_arr[fd].inode > 63){
        return -1;
    }
    dir_entry curr_entry; //dir_entry variable to store current entry
    // check whether reach to the end of valid directory
    if (read_dentry_by_index(curr_PCB[curr_rid]->fd_arr[fd].inode, &curr_entry) == -1) { //read the current dentry into curr_entry
        return 0;
     }
    //updatre dir_idex for next time read
    curr_PCB[curr_rid]->fd_arr[fd].inode++; // increment dir_idx once per function call

    int j; 
    int size = sizeof(curr_entry.file_name) > nbytes ? nbytes : sizeof(curr_entry.file_name); //swanky logic to set value of size, if greater than nbytes, then max size is nbytes, which should be 32
    for (j = 0; j < size; ++j) {

        ((int8_t*) buf)[j] = curr_entry.file_name[j]; //current file name entry   
    }
    ((int8_t*) buf)[j] = '\0';
    // inode* cur_node = (inode*) bootblock_start +curr_entry.inode_num + 1; //get inode from inode idx
    // terminal_printf("File Name: %s,File Type = %d, File Size = %d \n", buf, curr_entry.file_type, cur_node->length); // print to screen
    return j; // return # of bytes read
}

/* dir_write
 * Description: do nothing
 * Inputs:  fd
 *          buf: buffer to store the chars
 *          nbyte: length of the buf
 * Return Value:0 if success -1 if not
 * Function: do nothing
 */
int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes) {
    return -1;
}
/* read_dentry_by_name
 * Description: Find the directory entry of a file based on the file name
 * Inputs:   fname: name of the file
 *           dentry: directory entry need to be assigned 
 * Return Value:0 if success -1 if not
 * Function: update the dentry*/
int32_t read_dentry_by_name (const int8_t* fname, dir_entry* dentry){
    int file_size,i;
    //bad check
    if(dentry == NULL) return -1;
    file_size = strlen((int8_t*)fname);
    //check whether the fname is a correct pointer
    if(file_size > FILENAME_SIZE) return -1;
    if(file_size == 0) return -1;
    //loop through the dentry to match the file
    for(i = 0; i < (bootblock_start->dir_entry_num); i++) {
        // using strncmp to check whether the file names are the same for two strings
        if(strncmp((int8_t*)bootblock_start->dir_entries_63[i].file_name,(int8_t*)fname,FILENAME_SIZE) == 0) {
            *dentry = bootblock_start->dir_entries_63[i];
            return 0;
        }
    }
    // set default as failed
    return -1;
}
/* read_dentry_by_index
 * Description: Find the directory entry of a file based on the index of dentry
 * Inputs:   index: index of dentry in boot block
 *           dentry: directory entry need to be assigned 
 * Return Value:0 if success -1 if not
 * Function: update the dentry*/
int32_t read_dentry_by_index (uint32_t index, dir_entry* dentry){
    //bad check
    if(index >= (DIR_ENTRY_SIZE - 1) || bootblock_start->dir_entry_num <= index) return -1;

    // assign the dentry pointer
    *dentry = bootblock_start->dir_entries_63[index];
    return 0;
}
/* read_data
 * Description:  read the data of a file into a buffer
 * Inputs:   inode_idx : the index of the inode 
 *           buf : the buffer to save the data from file
 *           offset : starting position of the file
 *           length : the length of the buffer   
 * Return Value: 0 if end file reached
 *               byt_counter: the number of bytes read 
 *               -1 if failed
 * Function: read the data into the buffer*/
int32_t read_data (uint32_t inode_idx, uint32_t offset, uint8_t* buf, uint32_t length){
    int byt_count,block_used,block_now;
    uint32_t position, block_idx, block_cur_position;
    //bad check 
    if(bootblock_start->inodes_num <= inode_idx) return -1;
    //locate to current inode address
    inode* cur_node = (inode*) bootblock_start + inode_idx + 1; 
    //check for offset
    if(offset > cur_node->length) return -1;
    //initilize the position
    block_used = offset / DATA_BLOCK_SIZE;
    block_now = cur_node->data_block_num[block_used];
    //check if current block is out of range
    if(block_now < 0 || block_now >= (bootblock_start->data_blocks_num)) return -1;
    //iterate through the buf size 
    for(byt_count = 0; byt_count < length; byt_count++){
        position = offset+ byt_count;
        //first check if reaching to the end of file
        if(position >= cur_node->length) break;
        //locate the #th block currently in
        block_idx = position / DATA_BLOCK_SIZE;
        //locate to the current data_block
        data_block* cur_data_block = (data_block*)bootblock_start + (bootblock_start->inodes_num + 1) + cur_node->data_block_num[block_idx];
        // locate the corrent position in one data_block
        block_cur_position = position - block_idx * DATA_BLOCK_SIZE;       //position & (DATA_BLOCK_SIZE -1); Question
        // copy each data from datablock to the buffer
        *(buf + byt_count) = cur_data_block->data[block_cur_position];
    }
    return byt_count;
}






