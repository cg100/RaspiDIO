#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpd/client.h"
#include "radio.h"

#define RADIOSTATION_COUNT 2

struct mpd_connection *conn = NULL;

struct Radiostation rs[RADIOSTATION_COUNT];

void init_mpd()
{
	int i = 0;
	struct mpd_status* st;

	conn = mpd_connection_new("localhost", 6600, 0);
	if (conn == NULL)
		return;

	mpd_send_status(conn);
	st = mpd_recv_status(conn);
	cur_volume = mpd_status_get_volume(st);

	rs[0].RadioURL = "http://1live.akacast.akamaistream.net/7/706/119434/v1/gnl.akacast.akamaistream.net/1live";
	rs[0].RadioName = "1Live";
	rs[0].ID = 1;


	rs[1].RadioURL = "http://108.61.73.117:10002";
	rs[1].RadioName = "181.fm - The Eagle";
	rs[1].ID = 2;


	mpd_send_clear(conn);
	mpd_response_finish(conn);

	for(i = 0; i < RADIOSTATION_COUNT; i++)
	{
		mpd_run_add(conn, rs[i].RadioURL);
	}

	//mpd_send_play(conn);
	//mpd_response_finish(conn);
}

void start_mpd()
{
	mpd_send_play(conn);
	mpd_response_finish(conn);
}

void stop_mpd()
{
	mpd_send_stop(conn);
	mpd_response_finish(conn);
}

void set_volume(int vol)
{
	if((vol < 0) || (vol > 100))
		return;
	cur_volume = vol;
	mpd_send_set_volume(conn, cur_volume);
	mpd_response_finish(conn);
}

void set_volume_rel(int vol)
{
	if((cur_volume + vol) > 100 || (cur_volume + vol) < 0)
		return;
	cur_volume += vol;
	mpd_send_set_volume(conn, cur_volume);
	mpd_response_finish(conn);
}

int get_volume()
{
	return cur_volume;
}

void change_radiostation(int id)
{
	mpd_send_play_pos(conn, id);
	mpd_response_finish(conn);
}

char** get_senderlist()
{
	int i = 0;

	char** radiostations = 0;
	radiostations = (char**)malloc(RADIOSTATION_COUNT * sizeof(char*));

	for(i = 0; i < RADIOSTATION_COUNT; i++)
	{
		radiostations[i] = rs[i].RadioName;
	}

	return radiostations;
}

void close_mpd()
{
	mpd_send_stop(conn);
	mpd_response_finish(conn);
	mpd_send_clear(conn);
	mpd_response_finish(conn);
	mpd_connection_free(conn);
}

char* get_current_songtitle()
{
	struct mpd_song* song;
	char* title;
	char* cur_song = 0;

	song = mpd_run_current_song(conn);

	if (song != NULL) {
		title = (char*)mpd_song_get_tag(song, MPD_TAG_TITLE, 0);

		if(title != NULL)
		{
			cur_song = (char*)calloc(strlen(title), sizeof(char));
			strcpy(cur_song, title);
		}

		mpd_song_free(song);
	}

	return cur_song;
}

char* get_current_songartist()
{
	struct mpd_song* song;
	char* title;
	char* cur_song = 0;

	song = mpd_run_current_song(conn);

	if (song != NULL) {
		title = (char*)mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);

		if(title != NULL)
		{
			cur_song = (char*)calloc(strlen(title), sizeof(char));
			strcpy(cur_song, title);
		}

		mpd_song_free(song);
	}

	return cur_song;
}
