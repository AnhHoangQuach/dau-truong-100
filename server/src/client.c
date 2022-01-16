#include <stdio.h>
#include <gtk/gtk.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#include "../libs/protocol.h"
#include "../libs/tool.h"
#include "../libs/valid.h"

#define BUFF_SIZE 1024

// Question *ques = (Question *)malloc(sizeof(Question));
// Request *request = (Request *)malloc(sizeof(Request));
// Response *response = (Response *)malloc(sizeof(Response));
// Information *infor = (Information *)malloc(sizeof(Information));

// gameplay for normal client
void gamePlayForNormalTutorial()
{
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nGameplay Tutorial(Choose answer): ");
    printf("\n\tAnswer syntax: ANSWER answer");
    printf("\n-------------------Đấu trường 100-------------------");
    printf("\nInput to syntax: \n");
}
// tutorial choose topic
void chooseTopicLevel()
{
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nGameplay Tutorial (Choose level): ");
    printf("\n\tChoose Topic Level syntax: TOPIC level (level: EASY, NORMAL, HARD)");
    printf("\n--------------------------------------------------");
    printf("\nInput to syntax: \n");
}
// game play for special player
void gamePlayForSpecialTutorial()
{
    printf("-------------------Đấu trường 100-------------------\n");
    printf("\nGameplay Tutorial(Choose answer): ");
    printf("\n\tChoose Answer syntax: ANSWER answer");
    printf("\n\tUse Help syntax: HELP");
    printf("\n--------------------------------------------------");
    printf("\nInput to syntax: \n");
}
void showQuestion(Question *question)
{
    printf("\n%s", question->question);
    printf("\n%s", question->answer1);
    printf("\n%s", question->answer2);
    printf("\n%s", question->answer3);
    printf("\n%s", question->answer4);
}
GtkWidget *username_widget;
GtkWidget *password_widget;
GtkWidget *index_;

GAMEPLAY_STATUS status = UNAUTH;
int clickedSubmit = 0;
int client_sock, servPort;
GtkWidget *Login_Username;
int lucky;
char name[30], luckyPlayer[BUFF_SIZE], topic[BUFF_SIZE];
Question *ques = NULL;
Request *request = NULL;
Response *response = NULL;
char buff[BUFF_SIZE];
char pass[30];
GtkWidget *Login_Password;
GtkWidget *waiting_window;
GtkWidget *waiting_mess;

void clickedToLogin(GtkButton *login, gpointer data)
{
    GtkBuilder *builder;
    builder = gtk_builder_new_from_file("/home/thien/Downloads/dau-truong-100/View.glade");
    Login_Username = GTK_WIDGET(gtk_builder_get_object(builder, "Login_Username"));
    gtk_builder_connect_signals(builder, NULL);
    username_widget = GTK_WIDGET(gtk_builder_get_object(builder, "username"));
    g_signal_connect(Login_Username, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_object_unref(builder);
    gtk_widget_show(Login_Username);
}

void clickedToUserSubmit(GtkButton *Submit, gpointer data)
{
    strcpy(name, "USER ");
    strcat(name, gtk_entry_get_text(username_widget));
    request = (Request *)malloc(sizeof(Request));
    response = (Response *)malloc(sizeof(Response));
    setOpcodeRequest(request, name);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    strcpy(buff, readMessageResponse(response));
    status = response->status;
    GtkBuilder *builder;
    builder = gtk_builder_new_from_file("/home/thien/Downloads/dau-truong-100/View.glade");
    Login_Password = GTK_WIDGET(gtk_builder_get_object(builder, "Login_Password"));
    gtk_builder_connect_signals(builder, NULL);
    password_widget = GTK_WIDGET(gtk_builder_get_object(builder, "password"));
    buff[strlen(buff) - 1] = '\0';
    if (strcmp(buff, "Username is correct ") != 0)
    {
        show_message(Login_Username, GTK_MESSAGE_INFO, "Thong bao", buff);
    }
    else
    {
        gtk_widget_hide(index_);
        gtk_widget_show(Login_Password);
    }
    g_signal_connect(Login_Password, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_object_unref(builder);
}

void show_message(GtkWidget *parent, GtkMessageType type, char *mms, char *content)
{
    GtkWidget *mdialog;
    mdialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     type,
                                     GTK_BUTTONS_OK,
                                     "%s", mms);
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(mdialog), "%s", content);
    gtk_dialog_run(GTK_DIALOG(mdialog));
    gtk_widget_destroy(mdialog);
}

void clickedToPassSubmit(GtkButton *Submit, gpointer data)
{
    strcpy(pass, "PASS ");
    strcat(pass, gtk_entry_get_text(password_widget));
    request = (Request *)malloc(sizeof(Request));
    response = (Response *)malloc(sizeof(Response));
    setOpcodeRequest(request, pass);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    strcpy(buff, readMessageResponse(response));
    if (response->code == PASSWORD_CORRECT)
        gtk_widget_hide(Login_Username);
    status = response->status;
    buff[strlen(buff) - 1] = '\0';
    GtkBuilder *builder;
    GtkWidget *yourname;
    builder = gtk_builder_new_from_file("/home/thien/Downloads/dau-truong-100/View.glade");
    waiting_window = GTK_WIDGET(gtk_builder_get_object(builder, "waiting_window"));
    gtk_builder_connect_signals(builder, NULL);
    waiting_mess = GTK_WIDGET(gtk_builder_get_object(builder, "waiting_mess"));
    yourname = GTK_WIDGET(gtk_builder_get_object(builder, "yourname"));
    if (strcmp(buff, "Waiting other player... ") == 0)
    {
        gtk_entry_set_text(GTK_ENTRY(waiting_mess), "Waiting other player... ");
        gtk_label_set_text(GTK_LABEL(yourname), name);
        gtk_widget_show(waiting_window);
        g_signal_connect(waiting_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        g_object_unref(builder);
        gtk_widget_hide(Login_Password);
    }
 
    else
        show_message(Login_Password, GTK_MESSAGE_INFO, "Thong bao", buff);
}

void clickedToWaitingOk(GtkButton *Ok, gpointer data)
{
    requestGet(client_sock);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    status = response->status;
    strcpy(buff, readMessageResponse(response));
    buff[strlen(buff) - 1] = '\0';
    if (response->status == WAITING_QUESTION)
    {
        strcat(buff, "Nguoi choi may man do la ");
        gtk_entry_set_text(GTK_ENTRY(waiting_mess), strcat(buff, response->data));
        memset(luckyPlayer, '\0', (strlen(luckyPlayer) + 1));
        strcpy(luckyPlayer, response->data);
        if (strcmp(luckyPlayer, name) == 0)
            lucky = TRUE;
        else
            lucky = FALSE;
    }
    
    else{
        gtk_entry_set_text(GTK_ENTRY(waiting_mess), "Waiting other player... ");
    }

int main(int argc, char const *argv[])
{
    char username[BUFF_SIZE], topic[BUFF_SIZE];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received;
    char buff[BUFF_SIZE], code[BUFF_SIZE], data[BUFF_SIZE];
    ques = (Question *)malloc(sizeof(Question));
    request = (Request *)malloc(sizeof(Request));
    response = (Response *)malloc(sizeof(Response));
    Information *infor = (Information *)malloc(sizeof(Information));
    int lucky = FALSE, existQuestion = FALSE, help = FALSE;
    int questionNumber = 0;
    float score = 0;
    int inforamation = TRUE;
    int gameStatus = GAME_PLAYING;

    // gtk
    GtkBuilder *builder;

    //GtkWidget *k = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    if (argc != 3)
    {
        printf("\nParams incorrect\n");
    }
    else
    {
        // Check input : IP address & Port
        if (checkIPAndPort(argv[1], argv[2]) != 0)
        {
            // Step 1: Construct socket
            client_sock = socket(AF_INET, SOCK_STREAM, 0);
            // Step 2: Specify server address
            servPort = atoi(argv[2]);

            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(servPort);
            server_addr.sin_addr.s_addr = inet_addr(argv[1]);
            // Step 3: Request to connect server
            if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
            {
                printf("\nError!Can not connect to sever! Client exit imediately! ");
                return 0;
            }
            gtk_init(&argc, &argv);

            builder = gtk_builder_new_from_file("/home/thien/Downloads/dau-truong-100/View.glade");

            index_ = GTK_WIDGET(gtk_builder_get_object(builder, "Index"));
            gtk_builder_connect_signals(builder, NULL);

            g_signal_connect(index_, "destroy", G_CALLBACK(gtk_main_quit), NULL);

            g_object_unref(builder);

            gtk_widget_show(index_);
            gtk_main();
            // while (1)
            // {
            //     switch (status)
            //     {
            //     case UNAUTH:
            // send request
            //loginTutorial();
            //memset(buff, '\0', (strlen(buff) + 1));
            //fgets(buff, BUFF_SIZE, stdin);
            //buff[strlen(buff) - 1] = '\0';
            // setOpcodeRequest(request, name);
            // sendRequest(client_sock, request, sizeof(Request), 0);
            // // recv request
            // receiveResponse(client_sock, response, sizeof(Response), 0);
            // printf("%s\n", response->message);
            // memset(buff, '\0', (strlen(buff) + 1));
            // strcpy(buff, readMessageResponse(response));
            // printf("%s\n",buff);
            // GtkBuilder *build;
            // GtkWidget *message;
            // GtkWidget *server_message;
            // build = gtk_builder_new_from_file("/home/thien/dau-truong-100/View.glade");
            // server_message = GTK_WIDGET(gtk_builder_get_object(build, "server_message"));
            // gtk_builder_connect_signals(build, NULL);
            // message = GTK_WIDGET(gtk_builder_get_object(builder, "messa"));
            // gtk_entry_set_text(GTK_ENTRY(message), buff);
            // g_signal_connect(server_message, "destroy", G_CALLBACK(gtk_main_quit), NULL);
            // g_object_unref(build);
            // if(clickedSubmit == 1){
            //     gtk_widget_show(server_message);
            // }
            //readMessageResponse(response);

            status = response->status;
            if (response->data != NULL)
            {
                memset(username, '\0', (strlen(username) + 1));
                strcpy(username, response->data);
            }
            //break;
            // case WAITING_PLAYER:
            //     // send request
            //     requestGet(client_sock);
            //     receiveResponse(client_sock, response, sizeof(Response), 0);
            //     status = response->status;
            //     if (status == WAITING_QUESTION)
            //     {
            //         // read message
            //         readMessageResponse(response);
            //         memset(luckyPlayer, '\0', (strlen(luckyPlayer) + 1));
            //         strcpy(luckyPlayer, response->data);
            //         printf("Lucky player: %s\n", luckyPlayer);
            //         if (strcmp(luckyPlayer, username) == 0)
            //             lucky = TRUE;
            //         else
            //             lucky = FALSE;
            //     }
            //     break;
            // case WAITING_QUESTION:
            //     if (lucky == TRUE)
            //     {
            //         if (inforamation == FALSE)
            //         {
            //             // request get information of game
            //             requestCheckInformation(client_sock);
            //             // recv information of game
            //             receiveInformation(client_sock, infor, sizeof(Information), 0);
            //             if (infor->status == TRUE)
            //             {
            //                 inforamation = TRUE;
            //                 if (help == FALSE)
            //                 {
            //                     score = score + infor->score;
            //                     printf("Số người trả lời sai câu hỏi trên: %d\n", infor->playerAnswerWrong);
            //                     printf("Số điểm bạn nhận được: %.1f\n", infor->score);
            //                     printf("Số người cùng chơi: %d\n", infor->playerPlaying);
            //                     printf("Số điểm của bạn hiện tại: %.1f\n", score);
            //                 }
            //                 else
            //                 {
            //                     score = score - infor->score;
            //                     printf("Số điểm bạn nhận bị trừ: %.1f\n", infor->score);
            //                     printf("Số người trả lời sai câu hỏi trên: %d\n", infor->playerAnswerWrong);
            //                     printf("Số người cùng chơi: %d\n", infor->playerPlaying);
            //                     printf("Số điểm của bạn hiện tại: %.1f\n", score);
            //                     help = FALSE;
            //                 }
            //             }
            //         }
            //         else
            //         {
            //             // request check status of game: PLAYING or END
            //             requestGet(client_sock);
            //             receiveResponse(client_sock, response, sizeof(Response), 0);
            //             inforamation = FALSE;
            //             if (response->status == END_GAME)
            //             {
            //                 status = response->status;
            //                 readMessageResponse(response);
            //             }
            //             else
            //             {
            //                 // Choose topic
            //                 chooseTopicLevel();
            //                 memset(buff, '\0', (strlen(buff) + 1));
            //                 fgets(buff, BUFF_SIZE, stdin);
            //                 buff[strlen(buff) - 1] = '\0';
            //                 setOpcodeRequest(request, buff);
            //                 sendRequest(client_sock, request, sizeof(Request), 0);
            //                 // recv response
            //                 receiveResponse(client_sock, response, sizeof(Response), 0);
            //                 status = response->status;
            //                 if (status == PLAYING)
            //                 {
            //                     strcpy(topic, response->data);
            //                     readMessageResponse(response);
            //                 }
            //                 if (status == WAITING_QUESTION)
            //                 {
            //                     readMessageResponse(response);
            //                     inforamation = TRUE;
            //                 }
            //             }
            //         }
            //     }
            //     else
            //     {
            //         // check status of game: playing or end?
            //         requestGet(client_sock);
            //         receiveResponse(client_sock, response, sizeof(Response), 0);
            //         if (response->status == END_GAME)
            //         {
            //             status = response->status;
            //             readMessageResponse(response);
            //         }
            //         else
            //         {
            //             // rcv response from ser
            //             requestGet(client_sock);
            //             receiveResponse(client_sock, response, sizeof(Response), 0);
            //             status = response->status;
            //             if (status == PLAYING)
            //             {
            //                 strcpy(topic, response->data);
            //                 readMessageResponse(response);
            //             }
            //         }
            //     }
            //     break;
            // case PLAYING:
            //     if (lucky == TRUE)
            //     {
            //         if (existQuestion == TRUE)
            //         {
            //             printf("\nCâu hỏi số %d\n", questionNumber);
            //             printf("Chủ đề: %s", topic);
            //             showQuestion(ques);
            //             printf("\nCâu trả lời: \n");
            //             gamePlayForSpecialTutorial();
            //             // check if request->code == HELP
            //             if (strcmp(buff, "HELP") == 0)
            //             {
            //                 requestGetHelp(client_sock);
            //             }
            //             else
            //             {
            //                 // send request
            //                 memset(buff, '\0', (strlen(buff) + 1));
            //                 fgets(buff, BUFF_SIZE, stdin);
            //                 buff[strlen(buff) - 1] = '\0';

            //                 setOpcodeRequest(request, buff);
            //                 sendRequest(client_sock, request, sizeof(Request), 0);
            //             }
            //             // rcv request
            //             receiveResponse(client_sock, response, sizeof(Response), 0);

            //             status = response->status;
            //             readMessageResponse(response);
            //             if (status == WAITING_QUESTION) // error validate
            //             {
            //                 existQuestion = FALSE;
            //             }
            //             if (response->code == USER_USED_HINT_SUCCESS) // use hint
            //                 help = TRUE;
            //         }
            //         else
            //         {
            //             // request get question
            //             requestGet(client_sock);
            //             // rcv question
            //             receiveQuestion(client_sock, ques, sizeof(Question), 0);
            //             existQuestion = TRUE;
            //             questionNumber++;
            //         }
            //     }
            //     else
            //     {
            //         if (existQuestion == TRUE)
            //         {
            //             printf("\nCâu hỏi số %d\n", questionNumber);
            //             printf("Chủ đề: %s", topic);
            //             showQuestion(ques);
            //             printf("\nCâu trả lời: \n");
            //             gamePlayForNormalTutorial();
            //             // send answer
            //             memset(buff, '\0', (strlen(buff) + 1));
            //             fgets(buff, BUFF_SIZE, stdin);
            //             buff[strlen(buff) - 1] = '\0';
            //             setOpcodeRequest(request, buff);
            //             sendRequest(client_sock, request, sizeof(Request), 0);
            //             // recv response
            //             receiveResponse(client_sock, response, sizeof(Response), 0);
            //             status = response->status;
            //             if (status == WAITING_QUESTION)
            //             {
            //                 existQuestion = FALSE;
            //             }

            //             readMessageResponse(response);
            //         }
            //         else
            //         {
            //             // request get question
            //             requestGet(client_sock);
            //             // recv question
            //             receiveQuestion(client_sock, ques, sizeof(Question), 0);
            //             existQuestion = TRUE;
            //             questionNumber++;
            //         }
            //     }
            //     break;
            // case END_GAME:
            //     if (lucky == TRUE)
            //     {
            //         if (inforamation == FALSE)
            //         {
            //             // get result
            //             requestCheckInformation(client_sock);
            //             receiveInformation(client_sock, infor, sizeof(Information), 0);
            //             if (infor->status == TRUE)
            //             {
            //                 printf("Số điểm bạn nhận được là: %1.f\n", infor->score);
            //                 inforamation = TRUE;
            //             }
            //         }
            //         else
            //         {
            //             // request logout
            //             requestLogout(client_sock, username);
            //             receiveResponse(client_sock, response, sizeof(Response), 0);
            //             status = response->status;
            //             readMessageResponse(response);
            //             gameStatus = GAME_END;
            //         }
            //     }
            //     else
            //     {
            //         if (inforamation == TRUE)
            //         {
            //             inforamation = FALSE;
            //             // get information
            //             requestCheckInformation(client_sock);
            //             receiveInformation(client_sock, infor, sizeof(Information), 0);
            //             printf("Số điểm bạn nhận được là: %1.f\n", infor->score);
            //         }
            //         else
            //         {
            //             // request logout
            //             requestLogout(client_sock, username);
            //             receiveResponse(client_sock, response, sizeof(Response), 0);
            //             status = response->status;
            //             readMessageResponse(response);
            //             gameStatus = GAME_END;
            //         }
            //     }
            //     break;
            //}
            //if (gameStatus == GAME_END)
            //    break;
            //}
            // Step 5: Close socket
            close(client_sock);
            return 0;
        }
    }
}
