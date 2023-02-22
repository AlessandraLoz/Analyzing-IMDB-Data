/*
 * Course: CS216-00x
 * Project: Lab 5 (as second part of Project 1)
 * Purpose: it reads data from an input file which passes as command line argument
 *          then stores in an object of IMDB class (to represent imdb database)
 *          two movies are from the user-input, then performs case insensitive matching
 *          and match the movie titles from the database
 *          it then allowes user to input an actor/actress name, to search for co-actors in each movie which this actor/actress was in, in a well-formatted display
 * Author: Alessandra Lozano
 */
#include <iostream>
#include <limits>
#include <fstream>
#include <string>
#include <cctype>
#include <set>
#include <map>
#include <sstream>
#include "imdb.h"

using namespace std;


void analyzeMovies(const IMDB& imdb);

void analyzeCoActors(const IMDB& imdb);

int main(int argc, char* argv[])
{
    // Check whether the number of command line arguments is exactly one 
    if (argc != 2)
    {
        cout << "Warning: need exactly one command line argument." << endl;
        cout << "Usage: " << argv[0] << " inputfile_name" << endl;
        return 1;
    }
    ifstream in_file;
    in_file.open(argv[1]);

    // Check whether the input file can be open successfully or not
    if (!in_file.good())
    {
        cout << "Warning: cannot open file named " << argv[1] << "!" << endl;
        return 2;
    }

    // Read data from the input file, assume "good data" from the input file
    // each line of the input file: actor name, movie1, movie2, ..., movien
    // read one line at a time then tokenize it:
    // create a pair <actor_name, the set of movieTitles which the actor was in>
    //        and insert to IMDB object
    IMDB cs216_imdb;

    while (!in_file.eof())
    {
        string line;
        getline(in_file, line);
        string actorName, movieTitle;
        istringstream iss(line);
        getline(iss, actorName,';');

        // Extract extra white space
        iss>>ws;

        // Create a new set of movies associated with name
        set<string> movies;

        while (getline(iss, movieTitle, ';'))
        {
            movies.insert(movieTitle);
            // extract white space 
            iss>>ws;
        }
        // insert the pair to IMDB object: cs216_imdb
        cs216_imdb.insert_an_actor(actorName, movies);
    }
    // close the input file
    in_file.close();

    int option;
    while (true)
    {
        bool validInput = true;
        cout << "This application stores information about Actors and their Movies, please choose your option (Enter Q or q to quit):" << endl;
        cout << "1. Actors in Movies" << endl;
        cout << "2. Actors and co-actors" << endl;
        cin >> option;

        if (cin.fail())
        {
            cin.clear();
            string input_to_check;
            cin >> input_to_check;
            if (input_to_check == "Q" || input_to_check == "q")
                break;
            else
            {
                cout << "Expecting a number as the option!" << endl;
                validInput = false;
            }
        }
        // get rid of anything left in the input stream
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        //is a valid input option
        if (validInput)
        {
            switch (option)
            {
                 case 1:  
                        analyzeMovies(cs216_imdb);
                        break;
                case 2:  analyzeCoActors(cs216_imdb);
                        break;
                default:
                        cout << "Invalid option number!" << endl;
            }
            
            

        }
    }
    cout << "Thank you for using my program, bye..." << endl;
    return 0;
}

/*
 * Purpose: this function asks the user to type an actor's name
 * then check if the user-input name is valid(it is valid if it is in the database stored in the passing in object
 * if it is valid, display each movie which this actor is in, the co-actors who appeared in the same movie
 *@param imdb, the object of IMDB class: stores all information to look up (const call by reference)
 *@return void function
 */
void analyzeCoActors(const IMDB& imdb)
{
    string actor_name;
    cout << "Finding the co-actors of the actor by typing his/her name: ";
    getline(cin, actor_name);
    if (!imdb.isExistingActor(actor_name))
    {
        cout << "The actor name you entered is not in the database." << endl;
        return;
    }   

    set<string> movies_of_actor;
    movies_of_actor = imdb.find_movies_for_an_actor(actor_name);
    for (set<string>::iterator im = movies_of_actor.begin(); im != movies_of_actor.end(); im++)
    {
        cout << "The co-actors of " << actor_name << " in the movie \"" << *im << "\" are: " << endl;

        // Display all the co-actors in one movie
        set<string> coactors = imdb.find_actors_in_a_movie(*im);
        for (set<string>::iterator ia = coactors.begin(); ia != coactors.end(); ia++)
        {       
                if(*ia == actor_name)
                {
                    cout << " ";
                }
                else
                {
                    cout << *ia << endl;
                }
         }    
        cout << "***********************************" << endl;
    }
}    

void analyzeMovies(const IMDB& imdb)
{
    // Ask the user to input two movie titles to search for
    string movie1, movie2;
    cout << "Please input the first movie title: ";
    getline(cin,movie1);
    cout << "Please input the second movie title: ";
    getline(cin,movie2);
    // Check if both movie1 and movie2 can be matched with movies in the database
    // If either one is not, quit the program
    // Otherwise display all the actors in either of two movies
    //                   which are actually the union of two sets
    string matchedMovie1, matchedMovie2;
    matchedMovie1 = imdb.matchExistingMovie(movie1);
    matchedMovie2 = imdb.matchExistingMovie(movie2);
    // decide which one movie title is not valid
    bool notValid = false;
    if (matchedMovie1.length() == 0)
    {
        cout << "\" " << movie1 << " \"" << " is not a VALID movie title!" << endl;
        notValid = true;
    }
    if (matchedMovie2.length() == 0)
    {
        cout << "\" " << movie2 << " \"" << " is not a VALID movie title!" << endl;
        notValid = true;
    }
    if (notValid)
        return;
    // both movie titles are valid
    if (imdb.isExistingMovie(matchedMovie1) && imdb.isExistingMovie(matchedMovie2))
    {
        char menu_option; 
        set<string> all; //to store the union of actors_in_movie1 and actors_in_movie2
        set<string> common; //to store the intersection of actors_in_movie1 and actors_in_movie2
        set<string> all_in_one; //to store the  actors_in_movie1 and actors_in_movie2 that are only in one or the other
        set<string> actors_in_movie1 = imdb.find_actors_in_a_movie(matchedMovie1);
        set<string> actors_in_movie2 = imdb.find_actors_in_a_movie(matchedMovie2);

        cout << "Your input matches the following two movies: " <<endl;
        cout << matchedMovie1 << endl;
        cout << matchedMovie2 << endl;

       do 
      {
             cout << "Please input menu option (enter Q or q to quit) " << endl;
             cout << "A -- to print all the actors in either of the two movies." << endl;
             cout << "C -- to print common actors in both of the movies." << endl;
             cout << "O -- to print all the actors who are only in one movie, but not both." << endl;
             cin >> menu_option; 

             if (cin.fail())
             {
             cin.clear();
             cout << "Invalid Menu option!" << endl;
             }
           
             else
            {  
                 switch (menu_option)
                 {   
                     //print all actors in either one movie 
                     case 'A':
                         
                         all = actors_in_movie1;   
                         for (auto j = actors_in_movie2.begin(); j != actors_in_movie2.end(); j++)
                         {
                                 all.insert(*j);
                         }
                
                         // Display all the actors in either one movie
                         cout << "All the actors in either of the two movies:" << endl;
                         for (auto i = all.begin(); i != all.end(); i++)
                         {
                                cout << *i << endl;
                         }

                     //print all common actors in both movies
                     case 'C':
                        
                         common = actors_in_movie1;
                         for (auto k = actors_in_movie1.begin(); k != actors_in_movie1.end();k++)
                         {
                         
                             for (auto l = actors_in_movie2.begin(); l != actors_in_movie2.end();l++)
                             {
                                 if(*k == *l)
                                 {
                                   common.insert(*l);
                                 }
                             }

                         }

                         // Display all the actors in both movies
                         cout << "Common actors in both movies:" << endl;
                         for (auto n = common.begin(); n != common.end(); n++)
                         {
                                 cout << *n << endl;
                         }

                     case 'O':
                        
                        all_in_one = actors_in_movie1;
                          for (auto m = actors_in_movie1.begin(); m != actors_in_movie1.end();m++)
                          {

                                      for (auto p = actors_in_movie2.begin(); p != actors_in_movie2.end();p++)
                                      {
                                          if(*m != *p)
                                          {
                                                  all_in_one.insert(*p);
                                                  all_in_one.insert(*m);
                                          }
                                      }
                          }

                          // Display all the actors in only one movie, not both
                          cout << "Actors only in one of two movies:" << endl;
                          for (auto z = all.begin(); z != all.end(); z++)
                          {
                              cout << *z << endl;
                          }
                    
                     default:
                     {
                         cout << "Invalid Menu Option!" << endl;
                     }
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                 }
            }

       } while (menu_option != 'Q' || menu_option != 'q');
 
      
    }
}

