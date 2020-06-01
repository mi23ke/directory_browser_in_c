//Mike Urbano
//mxu2814
//Assignment 1
# include <stdio.h>
# include <curses.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <time.h>
# include <dirent.h>
# include <sys/wait.h>
# include <sys/stat.h>
# define N_MAX 2048           //max chars for names
# define MAX_ITEMS 1024       //max number of items
void print_dir_contents( DIR * dp, char dir[][N_MAX], char files[][N_MAX], char current_dir[N_MAX], int * num_f, int * num_d );
void edit_file ( char files[][N_MAX], int * num_f );
void change_dir( DIR * dp, char dir[][N_MAX], char current_dir[N_MAX], int * num_d);
void run_prog( char files[][N_MAX], int * num_f );
void sort_data( char files[][N_MAX], char dir[][N_MAX] );   //not implemented
void print_header();
int print_files( char files[][N_MAX], int * num_f, int row, int column );
int print_dir( char dir[][N_MAX], int * num_d, int row, int column );
int main()   {

    char command[100];
    char dir[MAX_ITEMS][N_MAX];     //char array to store working directory
    char files[MAX_ITEMS][N_MAX];   //char array to store file names
    char current_dir[N_MAX];
    getcwd(current_dir, N_MAX);
    DIR * dp;                         //directory pointer
    int i = 0, j, k;
    int n_files = 0, n_dir = 0;
    int *num_f = &n_files, *num_d = &n_dir;
    int row, column = 20;
    while ( strcasecmp( command, "q") != 0)
    {
        print_dir_contents( dp, dir, files, current_dir, num_f, num_d );               //displaying contents of directory
        initscr();
        row = 27;
        mvprintw( row, 0, "Operation: " );
        mvprintw( row++, column, "E -  Edit a File" );                                 //printing operation options
        mvprintw( row++, column, "R -  Run a File" );
        mvprintw( row++, column, "C -  Change Directory");
        mvprintw( row++, column, "S -  Sort Listing (Not implemented)");             //sort not implemented
        mvprintw( row++, column, "Q -  Quit");
        mvprintw( row + 1, column, "Please enter the letter for the desired operation: ");
        refresh();
        scanw( "%s*\n", command );
        endwin();
        if ( strcasecmp( command, "e" ) == 0) edit_file( files, num_f );
        if ( strcasecmp( command, "q" ) == 0) break;
        if ( strcasecmp( command, "c" ) == 0)
        {
           change_dir( dp, dir, current_dir, num_d );
           k = chdir( current_dir );
           getcwd( current_dir, N_MAX );
        }
        if ( strcasecmp( command, "r" ) == 0) run_prog( files, num_f );
    }
}
void print_header() {      //function that prints header information at the top of the screen
    time_t t;
    clear();
    char cwd[N_MAX];
    getcwd( cwd, N_MAX );
    mvprintw( 3, COLS/4, "Current Working Directory: %s", cwd );
    t = time(NULL);
    mvprintw( 4, COLS/4, "Process ID is: %d", getpid() );
    mvprintw( 5, COLS/4, "Time: %s", ctime(&t) );
}
int print_dir( char dir[][N_MAX], int * num_d, int row, int column ) {                   //this function prints the list of directories with scrolling
   int i, j = 0, k = *num_d, l = 0, page = 1, max_pages, selection = 0, max_items = 10; //displaying 10 items at a time, the user can then select a directory
   char option[100];                                                                    //which the function will then return to the calling procedure
   max_pages = *num_d/max_items+1;
   struct stat d_stat;
   while((selection<1 || selection>*num_d))
   {
      int n_row = row;
      clear();
      print_header();
      mvprintw( n_row, column, "Name:");
      mvprintw( n_row, column+40, "Size:");
      mvprintw( n_row, column+80, "Last Modified:");
      n_row++;
      mvprintw( n_row, 0, "Directories:" );
      for(i=0; (i<max_items && i < k); i++)
      {
         mvprintw(n_row + i, column, "%d. %s", l + 1, dir[l]);
         stat( dir[l], &d_stat);
         mvprintw( n_row+i, column+40, "%d Bytes", d_stat.st_size);
         mvprintw( n_row+i, column+80, "%s ", ctime(&d_stat.st_mtime));
         l++;
         refresh();
      }
      mvprintw( n_row + 1, 0, "Page %d of %d", page, max_pages);
      n_row = n_row + max_items +2;
      mvprintw( n_row, column, "Enter N for Next screen, P for previous screen, S to make a selction and B to go back to the previous menu:  ");
      scanw( "%s*\n", option);                                                        //display to get option from user, N for next screen, P for previous screen
      if( strcasecmp(option, "n") == 0 && page == max_pages)                          //S to make a selection and B to go back to the previous menu
      {
         l = *num_d - i;
         k = i;
      }
      if( strcasecmp(option,"n") == 0 && page < max_pages)                       //series of if statements control the display scrolling function
      {
         if(page == 1) k = *num_d - max_items;
         if(page > 1 && page < max_pages) k = k - max_items;
         page++;
      }
      if( strcasecmp(option, "p") == 0 && page == 1)
      {
         l = 0;
         k = *num_d;
      }
      if(strcasecmp(option, "p") == 0 && page > 1)
      {
         if( page < max_pages)
         {
            k = k + max_items;
            l = l - max_items - i;
         }
         if( page == max_pages)
         {
            k = i + max_items;
            l = *num_d - i - max_items; 
         }
         page--;
      }
      if( strcasecmp(option, "s") == 0 )
      {
         mvprintw( n_row + 2, column, "Enter the number of the directory you would like to select: " );   //get input from user when user is ready to make
         scanw( "%s*\n", option);                                                                            //a selection
         selection = atoi(option);
         if( selection > 0 && selection <= *num_d ); return selection;     //if choice is valid, return value to calling procedure
      }
      if( strcasecmp(option, "b") == 0 ) return -1;                          //if user enters B for back, return -1
      else l = *num_d - k;
   }
}

int print_files( char files[][N_MAX], int * num_f, int row, int column ) {                         //this function is the same as the previous function, except that it
   int i, j = 0, k = *num_f, l = 0, page = 1, max_pages, selection = 0, max_items = 10;            //displays files 
   char option[100];
   struct stat f_stat;                               //structure to display additional file data
   max_pages = *num_f/max_items + 1;
   while( (selection < 1 || selection > *num_f) )
   {
      int n_row = row;
      clear();
      print_header();
      mvprintw( n_row, column, "Name:");
      mvprintw( n_row, column+40, "Size:");
      mvprintw( n_row, column+80, "Last Modified:");
      n_row++;
      mvprintw( n_row, 0, "Files: Page: %d", page );
      for( i = 0; (i < max_items && i < k); i++ )
      {
         mvprintw( n_row + i, column, "%d. %s", l + 1, files[l] );
         stat( files[l], &f_stat);
         mvprintw( n_row+i, column+40, "%d Bytes", f_stat.st_size);
         //time_t t = f_stat.st_mtime;
         mvprintw( n_row+i, column+80, "%s ", ctime(&f_stat.st_mtime) );
         l++;
         refresh();
      }
      n_row = n_row + max_items + 2;
      mvprintw( n_row, column, "Enter N for next screen, P for previous screen, S to make a selection and B to go back to previous menu:  " );
      scanw( "%s*\n", option);
      if( strcasecmp(option, "n") == 0 && page == max_pages)
      {
         l = *num_f - i;
         k = i;
      }
      if( strcasecmp(option,"n") == 0 && page < max_pages)
      {
         if(page == 1) k = *num_f - max_items;
         if(page > 1 && page < max_pages) k = k - max_items;
         page++;
      }
     if( strcasecmp(option, "p") == 0 && page == 1)
     {
        l = 0;
        k = *num_f;
     }
     if( strcasecmp(option,"p") == 0 && page > 1)
     {
        if( page < max_pages)
        {
           k = k + max_items;
           l = l - max_items - i;
        }
        if( page == max_pages)
        {
           k = i + max_items;
           l = *num_f - i - max_items;
        }
        page--;
     }
     if( strcasecmp(option,"s") == 0 )
     {
        mvprintw( n_row + 2, column, "Enter the number of the file you would like to select: " );
        scanw( "%s*\n", option );
        selection = atoi( option );
        if( selection > 0 && selection <= *num_f ) return selection;
     }
     if( strcasecmp(option,"b") == 0 ) return -1;
     else l = *num_f - k;
   }
}
void print_dir_contents( DIR * dp, char dir[][N_MAX], char files[][N_MAX], char current_dir[N_MAX], int * num_f, int * num_d ) {
    *num_f = 0;
    *num_d = 0;
    struct dirent * dsp;
    dp = opendir( current_dir );
    initscr();
    print_header();
    int i = 0, j = 0, k = 0, l = 0, row = 8, column = 20;           //this function will display the contents of the current directory
    while ((dsp = readdir(dp)) != NULL )                            //but it will only display up to 5 items of each (files/directories)
    {                                                               //if there are more than 5 items, a message will be displayed to let the
        if ((dsp->d_type) & DT_DIR)                                 //user know that the full list can be viewed after selecting an operation
        {
            strcpy(dir[i], dsp->d_name);
            i++;
            *num_d = *num_d + 1;
        }
        if ((dsp->d_type) & DT_REG)
        {
            strcpy(files[k], dsp->d_name);
            k++;
            *num_f = *num_f + 1;
        }
    }
    closedir (dp);
    mvprintw( row, 0, "Directories:" );
    for( i = 0; (i < *num_d && i < 5); i++)
    {
        mvprintw( row+i, column, "%d. %s\n", j+1, dir[j]);
        j++;
    }
    if( *num_d > 5 ) mvprintw( row + j + 1, column, "**Select an option to see complete list of directories." );
    row = row + j + 3;
    mvprintw( row, 0, "Files:");
    if ( *num_f > 0 )
    {
       for( i = 0; (i < *num_f && i < 5); i++ )
       {
          mvprintw(row + i, column, "%d. %s\n", l+1, files[l]);
          l++;
       }
    }
    if ( *num_f > 5 ) mvprintw( row + l + 1, column, "**Select an option to see complete list of files in this directory." );
    if ( *num_f == 0 ) mvprintw( row, column, "There are no files in this directory." );
    refresh();
    endwin();
}
void edit_file( char files[][N_MAX], int * num_f )    {
    int i = 0, j = 0, choice = 0, row, column;
    char option[100];
    char command[100] = "pico ";
    char cwd[N_MAX];
    time_t t;
    t = time(NULL);
    getcwd( cwd, N_MAX );
    while( *num_f > 0 && ( choice < 1 || choice > *num_f) )
    {
       row = 8;
       column = 20;
       print_header();
       choice = print_files( files, num_f, row, column);
       if( choice == -1) return;
    }
    if( *num_f > 0 )
    {
       strcat( command, files[choice - 1] );
       system( command );
    }
    else mvprintw( row, 0, "There are no files to edit.");
}
void change_dir( DIR * dp, char dir[][N_MAX], char current_dir[N_MAX], int * num_d )
{
   int i = 0, j = 0, choice = 0, row, column;
   char option[100];
   while( choice < 1 || choice > *num_d )
   {
      print_header();
      row = 8;
      column = 20;
      choice = print_dir( dir, num_d, row, column );
      if( choice == -1) return;
      if( choice > 0 && choice <= *num_d) strcpy( current_dir, dir[choice-1]);
   }
}
void run_prog( char files[][N_MAX], int * num_f)
{
   char command[N_MAX]; 
   char argmt[100];
   char y_n[100];
   char option[100];
   int r, i, choice = 0, row, column;
   while( *num_f > 0 && ( choice < 1 || choice > *num_f) )                  //this function will allow a user to select a file to run
   {
      print_header();
      row = 8;
      column = 20;
      choice = print_files( files, num_f, row, column );
      if( choice == -1) return;
   }
   if ( fork() == 0 && *num_f > 0)
   {
      while( strcasecmp( y_n, "n" ) != 0 || strcasecmp( y_n, "y" ) != 0 )
      {
         print_header();
         row = 8;
         column = 20;
         mvprintw( row += 2, column, "Do you need to add an argument to the run command? (y/n): " );    //once a valid selection is made, the user will be asked
         scanw( "%s*\n", argmt);                                                                        //if any paraemters are required
         if( strcasecmp( argmt, "n" ) == 0 )
         {
            char * no_arg[] = {NULL};
            r = execv(files[choice-1], no_arg );
            if ( r == -1 )
            {
               mvprintw( LINES/2, COLS/4, "!!!!!!!Error running program, press the enter key to continue!!!!!!!!!!!!" ); //error message in case evecv fails
               scanw( "%c*\n", argmt );
               refresh();
            }
            _exit(0);
         }
         if( strcasecmp( argmt, "y" ) == 0 )
         {
            print_header();
            row = 8;
            column = 20;
            mvprintw( row += 2, column, "Enter the string argument: " );      //asking user for string of arguments to be added to run command
            getstr( argmt );
            strcat( command, files[choice - 1] );
            strcat( command, argmt );
            char * arg_p[3];
            arg_p[0] = files[choice - 1];
            arg_p[1] = argmt;
            arg_p[2] = NULL;
            refresh();
            r = execv( arg_p[0], arg_p );
            if ( r == -1 )
            {
               mvprintw( LINES/2, COLS/4, "!!!!!!!!!!!Error running program, press the enter key to continue!!!!!!!!!!!!1" ); //error message when
               scanw( "%c*\n", argmt );                                                                                       // execv fails
               refresh();
            }
            _exit(0);
         }
      }
   }
   else
   {
       if (r != -1) wait(NULL);
   }
}
