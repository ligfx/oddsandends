#include <stdlib.h>

typedef struct {
	int age;
	char *name;
} person;


person* person_new() {
	return malloc(sizeof(person));
}
int person_get_age(person *p) { return p->age; }
void person_set_age(person *p, int age) { p->age = age; }
char* person_get_name(person *p) { return p->name; }
void person_set_name(person *p, char *name) { p->name = name; }
