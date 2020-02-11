/*
 * password.c
 *
 *  Created on: Nov 11, 2019
 *      Author: motorcontrol
 */

#include "password.h"
#include "commands.h"
#include "terminal.h"
#include "string.h"
#include "stdio.h"
#include "conf_general.h"

static volatile uint8_t user_password[40];
static volatile uint8_t user_password_read[10];

#define EEPROM_ADDR_USER_PASSWORD 	1

static volatile bool system_locked = true;//this will be true until the user password is entered
static volatile bool password_is_erased = false;//this will be false if there is a valid password saved, or true if a new firmware was loaded, that erase the password
static volatile bool system_connection_alive = false;

static volatile uint32_t password_timeout_counter = 0;
static volatile uint32_t password_timeout_limit = 0;

static void terminal_cmd_enter_user_password(int argc, const char **argv) {
	(void)argc;
	(void)argv;

	if(system_locked){

		if(password_is_erased){
			//initialize password to Calibike
			password_is_erased = 0;
			strcpy(user_password,"Calibike");
			// Store data in eeprom
			conf_general_store_eeprom_var_hw((eeprom_var*)user_password, EEPROM_ADDR_USER_PASSWORD);
			conf_general_store_eeprom_var_hw((eeprom_var*)(user_password+4), EEPROM_ADDR_USER_PASSWORD+1);
		}

		if( argc == 2 ) {
			sscanf(argv[1], "%s", user_password);

			uint8_t pass_length = strlen((const char*)user_password);

			if( pass_length != 8){
				commands_printf("wrong password, it needs to be 8 characters long");
			}else{

				// Read stored password in eeprom
				conf_general_read_eeprom_var_hw((eeprom_var*)user_password_read,EEPROM_ADDR_USER_PASSWORD);
				conf_general_read_eeprom_var_hw((eeprom_var*)(user_password_read+4),EEPROM_ADDR_USER_PASSWORD+1);

				if( strncmp(user_password, user_password_read,8) == 0){
					system_locked = false;
					commands_printf("good password --> system unlocked");
					password_timeout_configure(300000);//5 minutes in msec
				}else{
					commands_printf("wrong password, it does not match current password ---> system keeps locked");
				}

			}

		}
		else {
			commands_printf("1 argument required. For example: new_user_password Calibike");
			commands_printf(" ");
		}
	}else{
		commands_printf("system is already unlocked.\r\n");
	}

	commands_printf(" ");
	return;
}

static void terminal_cmd_new_user_password(int argc, const char **argv) {
	(void)argc;
	(void)argv;

	if( system_locked ){
		commands_printf("system is locked, first unlock it, then you will be able to load a new password.");
	}else{
		if( argc == 2 ) {

			sscanf(argv[1], "%s", user_password);

			uint8_t pass_length = strlen((const char*)user_password);

			if( pass_length != 8){
				commands_printf("wrong password, it needs to be 8 characters long");
			}else{
				commands_printf("good password, User password will change to: %s", user_password);

				// Store data in eeprom
				conf_general_store_eeprom_var_hw((eeprom_var*)user_password, EEPROM_ADDR_USER_PASSWORD);
				conf_general_store_eeprom_var_hw((eeprom_var*)(user_password+4), EEPROM_ADDR_USER_PASSWORD+1);

				//read back written data
				conf_general_read_eeprom_var_hw((eeprom_var*)user_password_read,EEPROM_ADDR_USER_PASSWORD);
				conf_general_read_eeprom_var_hw((eeprom_var*)(user_password_read+4),EEPROM_ADDR_USER_PASSWORD+1);

				commands_printf("password saved:%s", user_password_read);
			}

		}
		else {
			commands_printf("1 argument required. For example: new_user_password Calibike");
			commands_printf(" ");
		}
	}
	commands_printf(" ");
	return;
}

static void terminal_cmd_lock_system(int argc, const char **argv) {

	system_locked = true;
	commands_printf("system has been locked\r\n");
	return;
}

void password_init(void){
	// Register terminal callbacks

	terminal_register_command_callback(
			"unlock",
			"Enter user password to unlock system, example: unlock <password>",
			0,
			terminal_cmd_enter_user_password);

	terminal_register_command_callback(
			"setpw",
			"Set a new user password for lock function, that must be 8 characters long, example: setpw <password>",
			0,
			terminal_cmd_new_user_password);

	terminal_register_command_callback(
			"lock",
			"locks system with password set in memory",
			0,
			terminal_cmd_lock_system);

	// check if flash was erased during a firmware upgrade, then initialize the password to Calibike
	conf_general_read_eeprom_var_hw((eeprom_var*)user_password_read,EEPROM_ADDR_USER_PASSWORD);
	conf_general_read_eeprom_var_hw((eeprom_var*)(user_password_read+4),EEPROM_ADDR_USER_PASSWORD+1);

	if( (user_password_read[0] == 0) &&
		(user_password_read[1] == 0) &&
		(user_password_read[2] == 0) &&
		(user_password_read[3] == 0) &&
		(user_password_read[4] == 0) &&
		(user_password_read[5] == 0) &&
		(user_password_read[6] == 0) &&
		(user_password_read[7] == 0) ){

		password_is_erased = true;
	}

}

bool password_get_system_locked_flag(void){
	return system_locked;
}

void password_set_system_locked_flag( bool value){
	system_locked = value;
}


bool password_get_system_connection_alive(void){
	return system_connection_alive;
}

void password_set_system_connection_alive(bool value){
	system_connection_alive = value;
}

void password_timeout_deinit(void){
	password_timeout_limit = 0;
	password_timeout_counter = 0;
}

void password_timeout_configure( uint32_t timeout_msec ){
	password_timeout_limit = timeout_msec;
	password_timeout_counter = 0;
}

void password_timeout_reset(void){
	password_timeout_counter = 0;
}

void password_timeout_increment(uint16_t delta_msec){
	if( password_timeout_limit > 0 ){
		password_timeout_counter += delta_msec;//this is called from main timer that has a 10 msec tick
		if( password_timeout_counter > password_timeout_limit ){
			system_locked = true;
		}
	}
}
