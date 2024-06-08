#include <pthread.h>

struct station {

	int boarding;
	int waiting_passengers;
	pthread_mutex_t lock;
    pthread_cond_t train_arrived;
	pthread_cond_t passenger_boarded;
	pthread_cond_t passenger_entered;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);