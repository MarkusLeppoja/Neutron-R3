#ifndef __FLASH_H__
#define __FLASH_H__

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_FlashTransport.h>
#include <Adafruit_SPIFlash.h>
#include <SdFat.h>

#include <definitions.h>
#include <alerts.h>


/* @brief Flash File
 * What does it do - Enables writing / reading data to flash device.
 * All functions - init, create_file, open_file, close_file, write_file_line, read_and_display_all_content, remove_file
 * 
 * Note - SPI instance is custom defined, changing that will result in flash chip not being writable.
 * Flash chip uses alerts functionality to communicate it's state. Ensure that flash chip file isn't included in alert's file to avoid errors.
 * Whether flash chip is usable or not is determined by the sw_flash_chip_usability (boolean), which gets its state in init function. Otherwise the flash will be disabled.
*/



class flash_device
{
private:
    // Automatically updated variable that determines if flash is working correctly to be used
    boolean sw_flash_chip_usability;
public:

    // @brief Begins the flash chip and the filesystem on it
    // Returns false if failed. True if succeeded
    int init(boolean _enable);

    // @brief if flash_init was successful then tries to create a new file
    // @param file_name the name of the file
    // @param file_format the format of the file (.csv, .txt)
    // Returns false if failed. True if succeeded.
    int create_file(String file_name, String file_format);

    // @brief if flash_init was successful then tries to open a file on given path
    // @param file_path the name + format of the file
    // @param file_oFlag what mode should the file be in (FILE_WRITE, FILE_READ)
    // Returns false if failed. True if succeeded
    int open_file(String file_path, oflag_t file_oFlag);

    // @brief If a file is open then it closes it, returning it's status
    int close_file();

    // @brief If file is open and flash_init was successful then writes given string to flash
    void write_file_line(String write_data);

    // @brief If flash_init was successful, then opens file on given path and prints all of it's content to Serial(1)
    int read_and_display_all_content(String file_path);

    // @brief If flash_init was successful, then removes the file on given path
    int remove_file(String file_path);

    // @brief Erases the entire flash chip.
    // @note This action is UNDOABLE
    int erase_chip();

    // @brief Returns if flash chip is usable
    int get_flash_chip_usability_status();
};
#endif