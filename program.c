/* A simplified community detection algorithm:
 *
 * Skeleton code written by Jianzhong Qi, May 2023
 * Edited by: [Zachary Williams 1172598]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define STAGE_NUM_ONE 1						  /* stage numbers */
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_HEADER "Stage %d\n==========\n" /* stage header format string */
#define MAX_USER 50
#define MAX_HASH_LEN 20
#define MAX_HASH_NUM 10
#define HASH_PER_LINE 5

typedef char data_t;							  /* to be modified for Stage 4 */

/* linked list type definitions below, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/
typedef struct node node_t;

struct node {
	data_t data[MAX_HASH_LEN];
	node_t *next;
};

typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

typedef struct {
	int date;
	int hashlen;
	char hash[MAX_HASH_NUM][MAX_HASH_LEN+1];
	int friend[MAX_USER];
	float strength[MAX_USER];
} user_t;

/****************************************************************/

/* function prototypes */
list_t *make_empty_list(void);
void free_list(list_t *list);
void print_list(list_t *list);
list_t *insert_unique_in_order(list_t *list, data_t value[]);

void print_stage_header(int stage_num);

int read_input(user_t *users);
void print_output(user_t *users, int largest_user, int user);
int stage_one(user_t *users);

void read_friend_rows(user_t *users, int usernum);
void compute_arrays(user_t *users, int usernum);
void compare_arrays(user_t *users, int u1, int u2, int usernum);
void s_o_c(user_t *users, int u1, int u2, int usernum);
void print_output_2(user_t *users);
void stage_two(user_t *users, int usernum);

void stage_three(user_t *users, int usernum);

void check_core_user(user_t *users, int usernum);
void get_hashtags(int user, int close_friend[], int core_count, user_t *users, list_t *list);
void print_stage_4_1(int user, int close_friend[], int core_count);
void stage_four(user_t *users, int usernum);

/* add your own function prototypes here */


/****************************************************************/

/* main function controls all the action; modify if needed */
int
main(int argc, char *argv[]) {
	/* add variables to hold the input data */
	user_t users[MAX_USER];
	int usernum;

	/* stage 1: read user profiles */
	usernum = stage_one(users); 
	
	/* stage 2: compute the strength of connection between u0 and u1 */
	stage_two(users, usernum);
	
	/* stage 3: compute the strength of connection for all user pairs */
	stage_three(users, usernum);
	
	/* stage 4: detect communities and topics of interest */
	stage_four(users, usernum);
	
	/* all done; take some rest */
	return 0;
}

/****************************************************************/

/* add your code below; you can also modify the function return type 
   and parameter list 
*/

/* read initial inputs and assign to struct, returns number of users */
int read_input(user_t *users) {

	char ch;
	char str[MAX_HASH_LEN];
	int usernum, dat, hashnum, total = 0, largest = 0, largest_user;

	ch = getchar();

	/* loop only when first character of line is a user */
	while (ch == 'u') {

		total = total + 1;

		scanf("%d", &usernum);
		scanf("%d", &dat);
		
		users[usernum].date = dat;
		hashnum = 0;
		/* loop through until newline, getchar will get whitespaces until newline */
		while ((ch = getchar()) != '\n') {

			/* scanf the string of hashtag and copy to struct */
			scanf("#%s", str);
			strcpy(users[usernum].hash[hashnum], str);

			hashnum = hashnum + 1;

		}
		/* check which user has most hashtags */
		if (hashnum > largest) {
			largest_user = usernum;
			largest = hashnum;
		}
		users[usernum].hashlen = hashnum;
		ch = getchar();
	}

	/* add stray character from friendship matrix to first user */
	users[0].friend[0] = ch - '0';

	print_output(users, largest_user, usernum + 1);
	return usernum + 1;
}

/* print outputs for stage 1 */
void print_output(user_t *users, int largest_user, int user) {

	printf("Number of users: %d\n", user);
	printf("u%d has the largest number of hashtags:\n", largest_user);

	printf("#%s", users[largest_user].hash[0]);
	/* loop through all hashtags and print them */
	for (int i = 1; i < users[largest_user].hashlen; i++) {
		printf(" #%s", users[largest_user].hash[i]);
	}
	printf("\n");
}

/* stage 1: read user profiles */
int stage_one(user_t *users) {
	/* add code for stage 1 */
	/* print stage header */
	print_stage_header(STAGE_NUM_ONE);
	int usernum;

	usernum = read_input(users);

	printf("\n");
	return usernum;
}

/* read rows of friendship matrix */
void read_friend_rows(user_t *users, int usernum) {
	int i, num;

	for(int z = 0; z < usernum; z++) {
		i = 0;
		/* we already read the first element of row 1 in stage 1 so let i = 1 */
		if (z == 0) {
			i = 1;
		}

		/* use while loop because i changes */
		while (i < usernum) {

			scanf("%d", &num);
			users[z].friend[i] = num;

			i++;
		}
	}
}

/* create array of friendship strength of each user */
void compute_arrays(user_t *users, int usernum) {
	for (int z = 0; z < usernum; z++) {

		for (int i = 0; i < usernum; i++) {
			/* call function using z and i as user indexes */
			s_o_c(users, z, i, usernum);
		}
	}
}

/* compare the friendship arrays of the two friends to calculate strength */
void compare_arrays(user_t *users, int u1, int u2, int usernum) {
	float intersect_count = 0, union_count = 0;

	/* iterate through length of users */
	for (int i = 0; i < usernum; i++) {

		/* if at least one user has a friend at this index */
		if (users[u1].friend[i] == 1 || users[u2].friend[i] == 1) {
			union_count++;

			/* if both users share that friend */
			if (users[u1].friend[i] == 1 && users[u2].friend[i] == 1) {
				intersect_count++;
			}
		}
	}
	/* compute and assign friend strength to new matrix */
	users[u1].strength[u2] = intersect_count/union_count;
}

/* function to compute strength of connection */
void s_o_c(user_t *users, int u1, int u2, int usernum) {

	/* if users are not friends make strength 0 */
	if (users[u1].friend[u2] == 0) {
		users[u1].strength[u2] = 0;
	}

	/* if they are friends calculate strength */
	else {
		compare_arrays(users, u1, u2, usernum);
	}
}

/* print output for stage 2 */
void print_output_2(user_t *users) {
	printf("Strength of connection between u0 and u1: %.2f\n", users[0].strength[1]);
}

/* stage 2: compute the strength of connection between u0 and u1 */
void 
stage_two(user_t *users, int usernum) {
	/* add code for stage 2 */
	/* print stage header */
	print_stage_header(STAGE_NUM_TWO);

	read_friend_rows(users, usernum);

	compute_arrays(users, usernum);

	print_output_2(users);

	printf("\n");
}

/* stage 3: compute the strength of connection for all user pairs */
void 
stage_three(user_t *users, int usernum) {
	/* add code for stage 3 */
	/* print stage header */
	print_stage_header(STAGE_NUM_THREE);
	/* algorithms are fun! */

	/* print strength matrix that was computed in stage 2 */
	for (int z = 0; z < usernum; z++) {
		printf("%.2f", users[z].strength[0]);
		for (int i = 1; i < usernum; i++) {
			printf(" %.2f", users[z].strength[i]);
		}
		printf("\n");
	}

	printf("\n");
}

/* find core users and their close friends */
void check_core_user(user_t *users, int usernum) {
	int core, core_count;
	float ths;
	int close_friend[usernum];
	list_t *list;

	list = make_empty_list();

	/* read final values */
	scanf("%f", &ths);
	scanf("%d", &core);

	/* iterate through whole strength matrix */
	for (int z = 0; z < usernum; z++) {
		core_count = 0;

		for (int i = 0; i < usernum; i++) {

			/* check if friendship strength if greater than ths for currect user */
			if (users[z].strength[i] > ths) {

				/* if so add user as close friend */
				close_friend[core_count] = i;
				core_count++;
			}
		}
		/* check if the number of close friends is great enough for core number */
		if (core_count > core) {
			get_hashtags(z, close_friend, core_count, users, list);

			print_stage_4_1(z, close_friend, core_count);
			print_list(list);

			free_list(list);
			list = make_empty_list();
		}
	}
}

/* simple function to simply run through core user and all their close friends' hashtags */
void get_hashtags(int user, int close_friend[], int core_count, user_t *users, list_t *list) {

	/* iterate through core user hashtags */
	for (int i = 0; i < users[user].hashlen; i++) {
		/* insert hashtag into linked list */
		list = insert_unique_in_order(list, users[user].hash[i]);
	}

	/* iterate through close friends hashtags */
	for (int z = 0; z < core_count; z++) {

		for (int x = 0; x < users[close_friend[z]].hashlen; x++) {
			/* insert hashtag into linked list */
			list = insert_unique_in_order(list, users[close_friend[z]].hash[x]);
		}
	}
}

/* print output for stage 4.1 */
void print_stage_4_1(int user, int close_friend[], int core_count) {
	printf("Stage 4.1. Core user: u%d; ", user);

	printf("close friends:");
	/* iterate through close friends */
	for (int i = 0; i < core_count; i++) {
		printf(" u%d", close_friend[i]);
	}
	printf("\n");
}

/* stage 4: detect communities and topics of interest */
void stage_four(user_t *users, int usernum) {
	/* add code for stage 4 */
	/* print stage header */
	print_stage_header(STAGE_NUM_FOUR);

	check_core_user(users, usernum);
}

/****************************************************************/
/* functions provided, adapt them as appropriate */

/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(STAGE_HEADER, stage_num);
}

/****************************************************************/
/* linked list implementation below, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/

/* create an empty list */
list_t
*make_empty_list(void) {
	list_t *list;

	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;

	return list;
}

/* free the memory allocated for a list (and its nodes) */
void
free_list(list_t *list) {
	node_t *curr, *prev;

	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}

	free(list);
}

/* insert a new data element into a linked list, keeping the
	data elements in the list unique and in alphabetical order */
list_t *insert_unique_in_order(list_t *list, data_t value[]) {
	node_t *new, *check, *prev;

	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	strcpy(new->data, value);

	if (list->foot==NULL) { /* this is the first insertion into the list */
		new->next = NULL;
		list->head = new;
		list->foot = new;
		return list;
	} /* the new hashtag is larger than foot value */
	if (strcmp(new -> data, list -> foot -> data) > 0) {
		new -> next = NULL;
		list -> foot -> next = new;
		list -> foot = new;
		return list;
	} /* same hashtag as head value */
	if (strcmp(new -> data, list -> head -> data) == 0) {
		return list;
	} /* if new hashtag is alphabetically smaller than head value */
	if (strcmp(new -> data, list -> head -> data) < 0) {
		new -> next = list -> head;
		list-> head = new;
		return list;
	}

	check = list -> head;
	while (check -> next != NULL) {
		/* previous node to check node */
		prev = check;
		check = check -> next;
		/* new hashtag is same as data in list */
		if (strcmp(new -> data, check -> data) == 0) {
			return list;
		} /* if new hashtag is alphabetically larger that check value */
		if (strcmp(new -> data, check -> data) > 0) {
			continue;
		} else { /* hashtag is smaller than check value, hence add between check node and prev node */
			new -> next = prev -> next;
			prev -> next = new;
			return list;
		}
	}
	return list;
}

/* print the data contents of a list */
void
print_list(list_t *list) {
	node_t *new;
	int count = 1;

	printf("Stage 4.2. Hashtags:\n");

	new = list -> head;

	while (new -> next != NULL) {

		/* newline per 5 hashtags */
		if (count == HASH_PER_LINE) {
			printf("#%s\n", new -> data);

			count = 1;

			new = new -> next;
			continue;
		}

		count++;

		printf("#%s ", new -> data);
		new = new -> next;
	}

	printf("#%s\n", new -> data);
}



