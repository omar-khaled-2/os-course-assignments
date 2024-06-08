#include <pthread.h>
#include "caltrain.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

void station_init(struct station *station) {
    station->waiting_passengers = 0;
    station->boarding = 0;
    pthread_mutex_init(&station->lock, NULL);
    pthread_cond_init(&station->train_arrived, NULL);
    pthread_cond_init(&station->passenger_entered, NULL);
    pthread_cond_init(&station->passenger_boarded, NULL);
}

void station_load_train(struct station *station, int count) {
    pthread_mutex_lock(&station->lock);


    while(count > 0 && station->waiting_passengers > 0){
        pthread_cond_signal(&station->train_arrived);
        count--;
        pthread_cond_wait(&station->passenger_entered, &station->lock);
    }

    while (station->boarding > 0) {
        pthread_cond_wait(&station->passenger_boarded, &station->lock);
    
    }


    pthread_mutex_unlock(&station->lock);
}

void station_wait_for_train(struct station *station) {
    pthread_mutex_lock(&station->lock);
    station->waiting_passengers++;


    pthread_cond_wait(&station->train_arrived, &station->lock);
    

    station->waiting_passengers--;
    station->boarding++;
    pthread_cond_signal(&station->passenger_entered);
    pthread_mutex_unlock(&station->lock);
}

void station_on_board(struct station *station) {
    pthread_mutex_lock(&station->lock);
    station->boarding--;
    pthread_cond_signal(&station->passenger_boarded);
    pthread_mutex_unlock(&station->lock);
}
