#include <stdio.h>
#include "mpd/client.h"
#include "radio.h"

#define RADIOSTATION_COUNT 1

struct mpd_connection *conn = NULL;

struct Radiostation rs[RADIOSTATION_COUNT];

void init_mpd()
{
	int i = 0;
	cur_volume = mpd_status_get_volume(conn);


	rs[0].RadioURL = "http://1live.akacast.akamaistream.net/7/706/119434/v1/gnl.akacast.akamaistream.net/1live";
	rs[0].RadioName = "1Live";
	rs[0].ID = 1;

	mpd_send_clear(conn);
	mpd_finish_response(conn);

	for(i = 0; i < RADIOSTATION_COUNT; i++)
	{
		mpd_run_add(conn, rs[i].RadioURL);
	}

	mpd_send_play(conn);
	mpd_finish_response(conn);
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
	if(vol < 2)
		cur_volume = vol;
	else
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
	mpd_finish_response(conn);
}

char** get_senderlist()
{
	int i = 0;

	char** radiostations;
	radiostations = malloc(RADIOSTATION_COUNT * sizeof(char*));

	for(i = 0; i < RADIOSTATION_COUNT; i++)
	{
		radiostations[i] = rs[i].RadioName;
	}

	return &radiostations;
}

void close_mpd()
{
	mpd_send_stop(conn);
	mpd_finish_response(conn);
	mpd_send_clear(conn);
	mpd_finish_response(conn);
	mpd_connection_free(conn);
}

char* get_current_songtitle()
{
	struct mpd_song* song;
	char* title;
	char* cur_song;

	song = mpd_run_current_song(conn);
	cur_song = malloc(sizeof(char*));

	if (song != NULL) {
		title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);

		if(title != NULL)
			strcpy(cur_song, title);

		mpd_song_free(song);
	}

	return cur_song;
}
