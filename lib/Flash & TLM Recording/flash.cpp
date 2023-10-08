#include "flash.h"

// Instances
SPIClassRP2040 spi_instance(spi0, e_pins::pin_spi_miso, e_pins::pin_cs_flash, e_pins::pin_spi_sck, e_pins::pin_spi_mosi);
Adafruit_FlashTransport_SPI flashTransport(e_pins::pin_cs_flash, &spi_instance);
Adafruit_SPIFlash flash(&flashTransport);
Alerts flash_alerts("Flash");
FatVolume fatfs;
File32 data_file; 

int flash_device::init(boolean _enable)
{
    if (!_enable) 
    {
        flash_alerts.create_alert(e_alert_type::warning, "Flash chip is disabled!");
        sw_flash_chip_usability = 0;
        return 1;
    }

    spi_instance.begin();

    // Check if a flash device exists
    if (!flash.begin())
    {
        flash_alerts.create_alert(e_alert_type::error, "Can't find flash chip.");
        sw_flash_chip_usability = 0;
        return 0;
    }
    
    // Log basic flash info
    flash_alerts.create_alert(e_alert_type::alert, "Flash chip JEDEC ID: 0x" + String(flash.getJEDECID(), HEX));

    // Check if a filesystem is present
    if (!fatfs.begin(&flash))
    {
        flash_alerts.create_alert(e_alert_type::error, "Failed to mount newly formatted filesystem!");
        sw_flash_chip_usability = 0;
        return 0;
    }

    // Indicate flash startup success and enable flash usability in software
    flash_alerts.create_alert(e_alert_type::success, "Flash startup complete");
    sw_flash_chip_usability = 1;

    return 1;
}

int flash_device::create_file(String file_name, String file_format)
{
    if (!sw_flash_chip_usability) return 0;

    //Check if file exists
    uint8_t exit_counter;
    while (fatfs.exists(file_name + file_format))
    {
        // Exit statement
        if (exit_counter > 5)
        {
            flash_alerts.create_alert(e_alert_type::error, "Couldn't find a suitable name after 5 tries. File name: " + file_name + file_format);
            return 0;
        }

        // Add an underscore to the file name to differentiate it
        file_name += "_";

        exit_counter++;
    }

    // Create a file
    data_file = fatfs.open(file_name + file_format, FILE_WRITE);

    // Check if file was created successfully
    if (!data_file)
    {
        flash_alerts.create_alert(e_alert_type::error, "Failed to create a file. File name: " + file_name + file_format);
        return 0;
    }

    // Indicate successful file creation
    flash_alerts.create_alert(e_alert_type::success, "Created file: " + file_name + file_format);
    return 1;
}

int flash_device::open_file(String file_path, oflag_t file_oFlag)
{
    if (!sw_flash_chip_usability) return 0;

    // Open file with given path
    data_file = fatfs.open(file_path, file_oFlag);

    if (!data_file)
    {
        flash_alerts.create_alert(e_alert_type::error, "Failed to open a file. File name: " + file_path);
        return 0;
    }

    // Indicate successful file opening
    flash_alerts.create_alert(e_alert_type::success, "Opened file: " + file_path + " With oFlag: " + String(file_oFlag));
    return 1;
}

int flash_device::close_file()
{
    if (!data_file.isOpen()) return 0;
    uint8_t file_close_status = data_file.close();
    flash_alerts.create_alert(e_alert_type::alert, "Closing file. File close status: " + String(file_close_status));
    return file_close_status;
}

void flash_device::write_file_line(String write_data)
{
    if (!sw_flash_chip_usability || !data_file.isOpen()) return;
    data_file.println(write_data.c_str());
}

int flash_device::read_and_display_all_content(String file_path)
{
    if (!sw_flash_chip_usability) return 0;
    if (!data_file.isOpen()) open_file(file_path, FILE_READ);

    // Display's all available content to Serial(1) 
    while (data_file.available())
    {
        char c = data_file.read();
        Serial.print(c);
        delayMicroseconds(50);
    }

    close_file();
    return 1;
}

int flash_device::remove_file(String file_path)
{
    if (!sw_flash_chip_usability || !fatfs.exists(file_path)) return 0;
    uint8_t remove_status = fatfs.remove(file_path);
    flash_alerts.create_alert(e_alert_type::alert, "Removing file named: " + file_path + " Remove status: " + String(remove_status));
    return remove_status;
}

int flash_device::get_flash_chip_usability_status()
{
    return sw_flash_chip_usability;
}