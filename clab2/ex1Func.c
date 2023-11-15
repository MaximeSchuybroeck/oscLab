typedef struct {
	short day, month;
	unsigned year;
} date_t;

// function declaration
void date_struct( int day, int month, int year, date_t *date);

// function definition
void date_struct( int day, int month, int year, date_t *date);{
	date_t dummy;
	dummy.day = (short)day;
	dummy.month = (short) month;
	dummy.year = (unsigned) year;
	date = &dummy;
}

int main (void) {
	int day, month, year;
	date_t d;
	printf("\nGive Day, month, year:");
	date_struct ( day, month, year, &d);
	printf("\ndate struct values: &d-&d-&d", d.day, d.month, d.year);
	return 0
}
