/*
 * radio.h
 *
 *  Created on: 12.06.2014
 *      Author: cgebing
 */

#ifndef RADIO_H_
#define RADIO_H_

struct Radiostation
{
	char* RadioURL;
	char* RadioName;
	int ID;
};

int cur_volume;

void init_mpd();
void start_mpd();
void stop_mpd();
void set_volume(int vol);
int get_volume();
void change_radiostation(int id);
char** get_senderlist();
void close_mpd();
char* get_current_songtitle();

#endif /* RADIO_H_ */
