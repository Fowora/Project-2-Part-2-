#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    return BENSCHILLIBOWLMenu[rand() % BENSCHILLIBOWLMenuLength];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
//   memory allocation  
  BENSCHILLIBOWL *newrest = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
  newrest->orders = NULL;
  newrest->current_size = 0;
  newrest->max_size = max_size;
  newrest->next_order_number = 1;
  newrest->expected_num_orders = expected_num_orders;
  
//   create mutex
  pthread_mutex_init(&(newrest->mutex), NULL);
  pthread_cond_init(&(newrest->can_add_orders), NULL);
  pthread_cond_init(&(newrest->can_get_orders), NULL);
  
  printf("Restaurant is open!\n");
  return newrest;
}


/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */

void CloseRestaurant(BENSCHILLIBOWL* bcb) {
  if (bcb->orders_handled != bcb->expected_num_orders){
    fprintf(stderr, "orders still available\n");
    exit(0);
  }
  pthread_mutex_destroy(&(bcb->mutex));
  free(bcb);
  printf("Restaurant is closed!\n");
}

/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    pthread_mutex_lock(&(bcb->mutex)); 
  
    while (isFull(bcb)){
      pthread_cond_wait(&(bcb->can_add_orders), &(bcb->mutex));
    }
    
    order->order_number = bcb->next_order_number;
    AddOrderToBack(&(bcb->orders), order);
    
    bcb->current_size++;
    bcb->next_order_number += 1;
  
     // broadcast new can get order
    pthread_cond_broadcast(&(bcb->can_get_orders));
  
    // unlock mutex
    pthread_mutex_unlock(&(bcb->mutex));
    
  
    return order->order_number;
}

/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex)); 
  
    while (isEmpty(bcb)){
      if (bcb->orders_handled >= bcb->expected_num_orders) {
          pthread_mutex_unlock(&(bcb->mutex));
          return NULL;
      }
      pthread_cond_wait(&(bcb->can_get_orders), &(bcb->mutex));
    }
  
    Order *order = bcb->orders;
    bcb->orders = bcb->orders->next;
    
    // update the current order size and orders handled
    bcb->current_size -= 1; 
    bcb->orders_handled += 1;
    
    // notify the process that it add get orders now
    pthread_cond_broadcast(&(bcb->can_add_orders));
        
    // release the lock.
    pthread_mutex_unlock(&(bcb->mutex));   
    return order;
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
  if (bcb->current_size == 0){
    return true;
  }
  else{
    return false;
  }
}

bool IsFull(BENSCHILLIBOWL* bcb) {
  return false;
}

/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
        *orders = order;
  } 
  else {
      Order *curr_order = *orders;
      while (curr_order->next) {
          curr_order = curr_order->next;
      }
      curr_order->next = order;
    }
}
