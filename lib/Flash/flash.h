#ifndef __FLASH_H__
#define __FLASH_H__

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_FlashTransport.h>
#include <Adafruit_SPIFlash.h>
#include <SdFat.h>
#include <alerts.h>

SPIClassRP2040 spi_flash_instance(spi0, e_pins::pin_spi_miso, e_pins::pin_cs_flash, e_pins::pin_spi_sck, e_pins::pin_spi_mosi);
Adafruit_FlashTransport_SPI flashTransport(e_pins::pin_cs_flash, &spi_flash_instance);
Adafruit_SPIFlash flash(&flashTransport);
Alerts flash_alerts("Flash");
FatVolume fatfs;
File32 data_file_0; 
File32 data_file_2; 

class FLASH_DEVICE_T;
extern FLASH_DEVICE_T flash_device;

class FLASH_DEVICE_T
{
public:

    // Begins the flash chip and the filesystem on it
    int init(boolean enable)
    {
        if (!enable)
        {
            is_flash_chip_usable = 0;
            flash_alerts.create_alert(e_alert_type::warning, "Flash chip is disabled!");
            return 1;
        }

        spi_flash_instance.begin();

        if (!flash.begin())
        {
            is_flash_chip_usable = 0;
            flash_alerts.create_alert(e_alert_type::error, "Can't connect with flash chip!");
            return 0;
        }

        flash_alerts.create_alert(e_alert_type::alert, "Flash chip JEDEC ID 0x" + String(flash.getJEDECID(), HEX));
        flash_alerts.create_alert(e_alert_type::alert, "Flash size " + String(flash.size() / 1024.f) + " KB");

        if (!fatfs.begin(&flash))
        {
            is_flash_chip_usable = 0;
            flash_alerts.create_alert(e_alert_type::error, "Failed to mount a filesystem!");
            return 0;
        }

        is_flash_chip_usable = 1;
        flash_alerts.create_alert(e_alert_type::success, "Flash startup was successful");
        return 1;
    }

    // Checks if a file exists on given path
    int does_file_exist(String path)
    {
        return fatfs.exists(path);
    }

    // Removes the file on given path
    int remove_file(String path)
    {
        if (!is_flash_chip_usable || !does_file_exist(path)) return 0;

        bool remove_status = fatfs.remove(path);
        flash_alerts.create_alert(e_alert_type::success, "Removing file " + String(path) + " Status " + String(remove_status));
        return remove_status;
    }

    // Erases the entire flash chip.
    // @note This action is UNREVERSABLE
    int erase_chip()
    {
        if (!flash.eraseChip())
        {
            flash_alerts.create_alert(e_alert_type::error, "Failed to erase flash chip");
            return 0;
        }

        flash.waitUntilReady();
        flash_alerts.create_alert(e_alert_type::success, "Erased flash chip");
        return 1;
    }

    // Casts all content of given file to Serial
    int print_file_content(String path, File32& file_instance)
    {
        if (!is_flash_chip_usable) return 0;

        flash_alerts.create_alert(e_alert_type::alert, "Attempting to print all file content. File path " + String(path));

        if (!file_instance.isOpen()) open_file(path, file_instance, FILE_READ);

        delay(5000);

        char read_data;
        while (file_instance.available())
        {
            read_data = file_instance.read();
            Serial.printf(String(read_data).c_str());

            delayMicroseconds(20);
        }

        close_file(file_instance);
        return 1;
    }

    // Closes given file
    int close_file(File32& file_instance)
    {
        if (!file_instance.isOpen()) return 1;
        bool close_file_status = file_instance.close();
        flash_alerts.create_alert(e_alert_type::alert, "Closing file. Status " + String(close_file_status));
        return close_file_status;
    }

    // Writes given data on given file
    void write_line(String data, File32& file_instance)
    {
        if (!is_flash_chip_usable || !file_instance.isOpen()) return;
        file_instance.printf(data.c_str());
    }

    // Opens a file on given path with a given oFlag
    int open_file(String path, File32& file_instance, oflag_t oFlag)
    {
        if (!is_flash_chip_usable) return 0;

        file_instance = fatfs.open(path, oFlag);

        if (!file_instance)
        {
            flash_alerts.create_alert(e_alert_type::error, "Failed to open file. Path " + String(path) + " oFLag " + String(oFlag));
            return 0;
        }

        flash_alerts.create_alert(e_alert_type::success, "Opened file " + String(path) + " with oFlag " + String(oFlag));
        return 1;
    }

    // Creates a new file. Errors out if a file exists on given path
    int create_file(String name, File32& file_instance, String format)
    {
        if (!is_flash_chip_usable) 
        {
            flash_alerts.create_alert(e_alert_type::error, "Failed to create file. Flash chip is disabled");
            return 0;
        }

        //Check if file exists
        if (fatfs.exists((name + format)))
        {
            flash_alerts.create_alert(e_alert_type::error, "File " + name + format + " already exists. Please remove the existing file to create a new file");
            return 0;
        }

        // Create a file
        file_instance = fatfs.open(name + format, FILE_WRITE);

        // Check if file was created successfully
        if (!file_instance.isOpen())
        {
            flash_alerts.create_alert(e_alert_type::error, "Failed to create a file " + name + format);
            return 0;
        }

        // Indicate successful file creation
        flash_alerts.create_alert(e_alert_type::success, "Created file " + String(name + format));
        return 1;
    }

    boolean is_file_open(File32& file_instance)
    {
        return file_instance.isOpen();
    }

    File32& get_file_instance(int file_count)
    {
        switch (file_count)
        {
            case 0:
                return data_file_0;
                break;
            case 2:
                return data_file_2;
                break;
            default: break;
        }
        return data_file_0;
        
    }

private:
    uint8_t is_flash_chip_usable : 1;
};
FLASH_DEVICE_T flash_device;

#endif