#include "list.h"
#include "extra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct ticket{
  char prioridad[6] ;
  char ID[100] ;
  char problema[1000] ;
  time_t hora ;
  char prioridad_chr ;
} ticket ;

// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     Sistema de Gestion de tickets");
  puts("========================================");
  puts("1) Registrar ticket");
  puts("2) Asignar prioridad a ticket");
  puts("3) Mostrar lista de tickets");
  puts("4) Atender al siguiente ticket"); 
  puts("5) Buscar ticket");
  puts("6) Salir");
}

// Funcion que busca ticket por ID y retorna el punto al ticket 
ticket * buscar_ticket(List * lista_tickets, char *id){
  ticket * actual = list_first(lista_tickets) ;
  while (actual != NULL){
    if (strcmp(actual->ID, id) == 0) return actual ; //  Retorna el puntero al ticket si hay un ticket con el ID ingresado
    actual = list_next(lista_tickets) ;
  }
  return NULL ; // Retorna NULL si no hay un ticket con el ID ingresado
}

// Ingresa el ticket y su problema
void registrar_ticket(List *tickets, List *PB) {
  ticket *tick = (ticket *) malloc(sizeof(ticket)) ;
  if (tick == NULL) exit(EXIT_FAILURE) ; 
  
  printf("Registrar nuevo ticket");
  printf("\nID Ticket: ") ;
  scanf(" %[^\n]s", tick->ID) ;
  if (buscar_ticket(tickets, tick->ID) != NULL) {
    printf("Este ticket ya ha sido registrado!\n") ;
    free(tick) ; // Se libera la memoria del ticket 
    return ;
  }
  printf("Descripcion del problema: ") ;
  scanf(" %[^\n]s", tick->problema) ;
  getchar() ;
  limpiarPantalla() ;
  strcpy(tick->prioridad, "Baja") ; // Se asigna prioridad baja 
  tick->prioridad_chr = '1' ;
  tick->hora = time(0) ; // Se guarda la hora local
  list_pushBack(PB, tick) ;
  list_pushBack(tickets,tick) ;
  printf("Ticket registrado correctamente!\n") ;
}

int compararTickets(void *a, void *b) {
  ticket *t1 = (ticket *)a;
  ticket *t2 = (ticket *)b;

  return t1->hora < t2->hora;
}

// Mueve el ticket a una prioridad nueva y se elimina de la lista de prioridad que estaba
void mover_ticket(ticket *tick, List *lista_agregar, List *lista_prioridad){

  ticket *actual = list_first(lista_prioridad) ;
  while (actual != tick){
    actual = list_next(lista_prioridad) ;
  }  
  ticket *aux = list_popCurrent(lista_prioridad) ;
  list_sortedInsert(lista_agregar, aux, compararTickets) ; // Ingresa el ticket por orden de ingreso

}

// Modifica la prioridad de un ticket y se mueve a su respectiva lista de prioridad
void asignar_prioridad(List *tickets, List *PB, List *PM, List *PA){
  char idTemp[100] ;
  char opcion ;
  printf("ID del ticket: ") ;
  
  scanf(" %s", idTemp) ;

  ticket *ticket_buscado = buscar_ticket(tickets, idTemp) ;
  while (ticket_buscado == NULL){
    char yn ;
    printf("Ticket no encontrado. Quieres asignarle prioridad a otro ticket?\n") ;
    printf("1) Si\n") ;
    printf("2) No\n") ;
    scanf(" %c", &yn) ;
    if (yn == '2') return ;
    limpiarPantalla() ;
    printf("ID del ticket: ") ;
    scanf(" %s", idTemp) ;
    ticket_buscado = buscar_ticket(tickets, idTemp) ;
  }

  printf("1) Prioridad Baja\n") ;
  printf("2) Prioridad Media\n") ;
  printf("3) Prioridad Alta\n") ;
  printf("Ingrese su opcion: ") ;
 
  scanf(" %c", &opcion) ;
  while (opcion < 49 || opcion > 51){
    printf("Opcion no valida, intente de nuevo :") ; // Opcion tiene que ser si o si entre 1 y 3
    scanf(" %c", &opcion) ;
  }
  if (ticket_buscado->prioridad_chr == opcion) {
    printf("El ticket ya tiene esa prioridad\n") ;  
    return ;
  }

  
  switch (opcion)
  {
    case '1':
      
      if (ticket_buscado->prioridad_chr == '2') mover_ticket(ticket_buscado, PB, PM) ; 
      else mover_ticket(ticket_buscado, PB, PA) ;
      strcpy(ticket_buscado->prioridad, "Baja") ;
      ticket_buscado->prioridad_chr = '1' ;
      break;
    case '2' :
      if (ticket_buscado->prioridad_chr == '1') mover_ticket(ticket_buscado, PM, PB) ; 
      else mover_ticket(ticket_buscado, PM, PA) ;
      strcpy(ticket_buscado->prioridad, "Media") ;
      ticket_buscado->prioridad_chr = '2' ;
      break;
    case '3' :
      
      if (ticket_buscado->prioridad_chr == '2') mover_ticket(ticket_buscado, PA, PM) ; 
      else mover_ticket(ticket_buscado, PA, PB) ;
      strcpy(ticket_buscado->prioridad, "Alta") ;
      ticket_buscado->prioridad_chr = '3' ;
      break;
  }
  printf("Prioridad asignada correctamente!\n") ;

}

// Muestra todos los tickets de la lista con su prioridad
void mostrarLista(List *L){
  List *lista_auxiliar = list_create() ;
  ticket *current = list_first(L) ;
  while (current != NULL)
  {
    printf("ID: %s prioridad = %s <-- ", current->ID, current->prioridad) ;
    current = list_next(L) ;
  }
}

// Muestra las tres listas por orden de prioridad
void mostrar_lista_tickets(List *listatickets, List *PB, List *PM, List *PA) {
  if (list_first(listatickets) == NULL) {
    printf("No se ha registrado ningun ticket\n") ;
    return ;
  }
  printf("Tickets en espera: \n");
  mostrarLista(PA) ;
  mostrarLista(PM) ;
  mostrarLista(PB) ; 
  printf("\n") ;
}


// Se busca el ticket y se eilima de la lista
void eliminarTicket(List *lista_tickets, ticket *tick) {
  ticket *current = list_first(lista_tickets) ;
  while (current != tick) {
    current = list_next(lista_tickets) ;
  }
  list_popCurrent(lista_tickets) ;
}

// Muestra toda la informacion del ticket
void mostrar_info_ticket(ticket *tick){
  struct tm * timeinfo;
  timeinfo = localtime(&tick->hora) ;
  printf("\nFecha y hora = %s", asctime(timeinfo)) ;
  printf("Problema = %s\n", tick->problema) ;
  printf("Prioridad %s\n", tick->prioridad) ;
}


// Se va revisando lista por lista por orden de prioridad e ingreso y se elimina el ticket que tiene que ser procesado.
void atender_ticket(List *lista_tickets, List *PB, List *PM, List *PA){
  int count = 0 ;
  if (queue_front(PA) != NULL) {
    ticket *tick = list_popFront(PA) ;
    printf("Siguiente ticket: ID = %s", tick->ID) ;
    mostrar_info_ticket(tick) ;
    eliminarTicket(lista_tickets, tick) ;
    count ++ ;
  }
  else if (queue_front(PM) != NULL) {
    ticket *tick = list_popFront(PM) ;
    printf("Siguiente ticket: ID = %s", tick->ID) ;
    mostrar_info_ticket(tick) ;
    eliminarTicket(lista_tickets, tick) ;
    count ++ ;
  }
  else if (queue_front(PB) != NULL){
    ticket *tick = list_popFront(PB) ;
    printf("Siguiente ticket: ID = %s", tick->ID) ;
    mostrar_info_ticket(tick) ;
    eliminarTicket(lista_tickets, tick) ;
    count ++ ;
  }
  if (count == 0) printf("No se ha registrado ningun ticket.\n") ;
}

// Se busca un ticket por ID y si existe se muestra toda la informacion guardada
void buscar_mostrar_ticket(List *ticketos){
  char idTicket[100] ;
  printf("ID del ticket: ") ;
  scanf(" %s", idTicket) ;
  ticket *tick = buscar_ticket(ticketos, idTicket) ;
  if (tick == NULL){
    printf("No existe un ticket con ese ID.\n") ;
    return ;
  }
  printf("\nInformacion Del ticket '%s':", tick->ID) ;
  mostrar_info_ticket(tick) ;

}

int main() {
  char opcion;
  List *tickets = list_create(); 
  List *p_baja = list_create(p_baja) ;
  List *p_media = list_create(p_media) ;
  List *p_alta = list_create(p_alta) ;

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opcion: ");
    scanf(" %c", &opcion); 

    switch (opcion) {
    case '1':
      limpiarPantalla() ;
      puts("Opcion seleccionada: 1) Registrar ticket") ;
      registrar_ticket(tickets, p_baja);
      break;
    case '2':
      limpiarPantalla() ;
      puts("Opcion seleccionada: 2) Asignar prioridad a ticket") ;
      asignar_prioridad(tickets, p_baja, p_media, p_alta) ;
      break;
    case '3':
      limpiarPantalla() ;
      mostrar_lista_tickets(tickets, p_baja, p_media, p_alta);
      break;
    case '4':
      limpiarPantalla() ;
      puts("Opcion seleccionada: 4) Atender al siguiente ticket") ;
      atender_ticket(tickets, p_baja, p_media, p_alta) ;
      break;
    case '5':
      limpiarPantalla() ;
      buscar_mostrar_ticket(tickets) ;
      break;
    case '6':
      puts("Saliendo del sistema de gestion de tickets...");
      break;
    default:
      puts("Opcion no valida. Por favor, intente de nuevo.");
    }
    presioneTeclaParaContinuar();

  } while (opcion != '6');

  list_clean(tickets);

  return 0;
}