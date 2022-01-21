#include <stdio.h>
#include <gtk/gtk.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include "../libs/protocol.h"
#include "../libs/tool.h"
#include "../libs/valid.h"

#define BUFF_SIZE 1024

GtkWidget *username_widget;
GtkWidget *password_widget;
GtkWidget *index_;

int clickhelp_button = 0;
int choose_lucky = 0;
int questionNumber = 0;
GAMEPLAY_STATUS status = UNAUTH;
int clickedSubmit = 0;
int client_sock, servPort;
GtkWidget *Login_Username;
int lucky;
char name[30], luckyPlayer[BUFF_SIZE], topic[BUFF_SIZE], buff[BUFF_SIZE], pass[30];
Question *ques = NULL;
Request *request = NULL;
Response *response = NULL;
Information *infor = NULL;
int existQuestion = FALSE;
float score = 0;
int gameStatus = GAME_PLAYING;
char register_line[BUFF_SIZE];
char buff1[BUFF_SIZE];
int information = TRUE;
int help = FALSE;
GtkWidget *Login_Password;
GtkWidget *GameView;
GtkWidget *TopicChoice;
GtkWidget *GameView1;
GtkWidget *waiting_window;
GtkWidget *waiting_mess;
GtkWidget *Register;
GtkWidget *user_regis;
GtkWidget *pass_regis;
GtkWidget *score_show;
GtkWidget *survival;
GtkWidget *wingame;
GtkWidget *user_win;
int ten = 0;
char scoreShow[BUFF_SIZE];
char survivalShow[BUFF_SIZE];
char dapan;

void clickedToLogin(GtkButton *login, gpointer data)
{
    GtkBuilder *builder;
    builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
    Login_Username = GTK_WIDGET(gtk_builder_get_object(builder, "Login_Username"));
    gtk_builder_connect_signals(builder, NULL);
    username_widget = GTK_WIDGET(gtk_builder_get_object(builder, "username"));
    g_signal_connect(Login_Username, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_object_unref(builder);
    gtk_widget_show(Login_Username);
}

void clickedToRegister(GtkButton *regist, gpointer data)
{
    GtkBuilder *builder;
    builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
    Register = GTK_WIDGET(gtk_builder_get_object(builder, "Register"));
    gtk_builder_connect_signals(builder, NULL);
    user_regis = GTK_WIDGET(gtk_builder_get_object(builder, "user_regis"));
    pass_regis = GTK_WIDGET(gtk_builder_get_object(builder, "pass_regis"));
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(Register, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_object_unref(builder);
    gtk_widget_show(Register);
}

void clickedToResigerSubmit(GtkButton *subm, gpointer data)
{
    strcpy(register_line, "REGISTER ");
    strcat(register_line, gtk_entry_get_text(user_regis));
    strcat(register_line, " ");
    strcat(register_line, gtk_entry_get_text(pass_regis));
    setOpcodeRequest(request, register_line);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    strcpy(buff, readMessageResponse(response));
    show_message(Register, GTK_MESSAGE_INFO, "Thong bao", buff);
    gtk_widget_hide(Register);
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
    builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
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
    strcpy(buff1, buff);
    buff[strlen(buff) - 1] = '\0';
    GtkBuilder *builder;
    GtkWidget *yourname;
    builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
    waiting_window = GTK_WIDGET(gtk_builder_get_object(builder, "waiting_window"));
    gtk_builder_connect_signals(builder, NULL);
    waiting_mess = GTK_WIDGET(gtk_builder_get_object(builder, "waiting_mess"));
    yourname = GTK_WIDGET(gtk_builder_get_object(builder, "yourname"));
    if (response->status == WAITING_PLAYER)
    {
        gtk_entry_set_text(GTK_ENTRY(waiting_mess), "Waiting other player... ");
        gtk_label_set_text(GTK_LABEL(yourname), name);
        gtk_widget_show(waiting_window);
        g_signal_connect(waiting_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        g_object_unref(builder);
        gtk_widget_hide(Login_Password);
    }

    else
        show_message(Login_Password, GTK_MESSAGE_INFO, "Thong bao", buff1);
}

void clickedToWaitingOk(GtkButton *Ok, gpointer data)
{

    if (choose_lucky == 0)
    {
        requestGet(client_sock);
        receiveResponse(client_sock, response, sizeof(Response), 0);
        if (response->status == WAITING_QUESTION)
        {
            memset(luckyPlayer, '\0', (strlen(luckyPlayer) + 1));
            strcpy(luckyPlayer, response->data);
            printf("%s\n", luckyPlayer);
            if (strcmp(luckyPlayer, gtk_entry_get_text(username_widget)) == 0)
                lucky = TRUE;
            else
                lucky = FALSE;

            choose_lucky = 1;
        }
    }
    if (response->status == WAITING_QUESTION && choose_lucky == 1)
    {
        infor = (Information *)malloc(sizeof(Information));
        if (lucky == TRUE)
        {

            if (information == FALSE)
            {
                requestCheckInformation(client_sock);
                receiveInformation(client_sock, infor, sizeof(Information), 0);
                if (infor->status == TRUE)
                {
                    information = TRUE;
                    if (help == FALSE)
                    {
                        float temp = atof(scoreShow);
                        gcvt(temp + infor->score, 6, scoreShow);
                    }
                    else
                    {
                        float temp = atof(scoreShow);
                        gcvt(temp - infor->score, 6, scoreShow);
                        help = FALSE;
                    }
                }
            }
            else
            {
                requestGet(client_sock);
                receiveResponse(client_sock, response, sizeof(Response), 0);
                information = FALSE;
                if (response->status == END_GAME)
                {
                    status = response->status;
                    GtkBuilder *builder;
                    char commandWin[BUFF_SIZE];
                    builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
                    wingame = GTK_WIDGET(gtk_builder_get_object(builder, "wingame"));
                    user_win = GTK_WIDGET(gtk_builder_get_object(builder, "user_win"));
                    gtk_widget_show(wingame);
                    sprintf(commandWin, "%s da chien thang", luckyPlayer);
                    gtk_label_set_text(GTK_LABEL(user_win), commandWin);
                }
                else
                {
                    GtkBuilder *builder;
                    builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
                    TopicChoice = GTK_WIDGET(gtk_builder_get_object(builder, "TopicChoice"));
                    gtk_builder_connect_signals(builder, NULL);
                    g_signal_connect(TopicChoice, "destroy", G_CALLBACK(gtk_main_quit), NULL);
                    g_object_unref(builder);
                    gtk_widget_show(TopicChoice);
                    gtk_widget_hide(waiting_window);
                }
            }
        }
        else
        {
            requestGet(client_sock);
            receiveResponse(client_sock, response, sizeof(Response), 0);
            if (response->status == END_GAME)
            {
                status = response->status;
            }
            else
            {
                // rcv response from ser
                requestGet(client_sock);
                receiveResponse(client_sock, response, sizeof(Response), 0);
                status = response->status;
                if (status == PLAYING)
                {
                    ques = (Question *)malloc(sizeof(Question));
                    requestGet(client_sock);
                    receiveQuestion(client_sock, ques, sizeof(Question), 0);
                    existQuestion = TRUE;
                    questionNumber++;
                    GtkBuilder *builder;
                    GtkWidget *question1, *ans_1, *ans_2, *ans_3, *ans_4;
                    builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
                    GameView1 = GTK_WIDGET(gtk_builder_get_object(builder, "GameView1"));
                    gtk_builder_connect_signals(builder, NULL);
                    question1 = GTK_WIDGET(gtk_builder_get_object(builder, "question1"));
                    ans_1 = GTK_WIDGET(gtk_builder_get_object(builder, "ans_1"));
                    ans_2 = GTK_WIDGET(gtk_builder_get_object(builder, "ans_2"));
                    ans_3 = GTK_WIDGET(gtk_builder_get_object(builder, "ans_3"));
                    ans_4 = GTK_WIDGET(gtk_builder_get_object(builder, "ans_4"));
                    gtk_label_set_text(GTK_LABEL(question1), ques->question);
                    gtk_label_set_text(GTK_LABEL(ans_1), ques->answer1);
                    gtk_label_set_text(GTK_LABEL(ans_2), ques->answer2);
                    gtk_label_set_text(GTK_LABEL(ans_3), ques->answer3);
                    gtk_label_set_text(GTK_LABEL(ans_4), ques->answer4);
                    g_signal_connect(GameView1, "destroy", G_CALLBACK(gtk_main_quit), NULL);
                    g_object_unref(builder);
                    gtk_widget_show(GameView1);
                    gtk_widget_hide(waiting_window);
                }
            }
        }
    }
    else
    {
        gtk_entry_set_text(GTK_ENTRY(waiting_mess), "Waiting other player... ");
    }
}

void chooseTopicEasy(GtkButton *easy, gpointer data)
{
    strcpy(buff, "TOPIC EASY");
    request = (Request *)malloc(sizeof(Request));
    response = (Response *)malloc(sizeof(Response));
    ques = (Question *)malloc(sizeof(Question));
    setOpcodeRequest(request, buff);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    status = response->status;
    if (status == PLAYING)
    {
        strcpy(topic, response->data);
        requestGet(client_sock);
        receiveQuestion(client_sock, ques, sizeof(Question), 0);
        existQuestion = TRUE;
        questionNumber++;
    }
    if (existQuestion == TRUE)
    {
        GtkBuilder *builder;
        GtkWidget *question, *mainuser_A, *mainuser_B, *mainuser_C, *mainuser_D;
        builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
        GameView = GTK_WIDGET(gtk_builder_get_object(builder, "GameView"));
        gtk_builder_connect_signals(builder, NULL);
        question = GTK_WIDGET(gtk_builder_get_object(builder, "question"));
        mainuser_A = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_A"));
        mainuser_B = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_B"));
        mainuser_C = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_C"));
        mainuser_D = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_D"));
        score_show = GTK_WIDGET(gtk_builder_get_object(builder, "score_show"));
        survival = GTK_WIDGET(gtk_builder_get_object(builder, "survival"));
        if (ten == 0)
        {
            gtk_label_set_text(GTK_LABEL(score_show), "0");
            gtk_label_set_text(GTK_LABEL(survival), "2");
            ten = 1;
        }
        else
        {
            gcvt(infor->score, 6, scoreShow);
            sprintf(survivalShow, "%d", infor->playerPlaying);
            gtk_label_set_text(GTK_LABEL(score_show), scoreShow);
            gtk_label_set_text(GTK_LABEL(survival), survivalShow);
        }
        gtk_label_set_text(GTK_LABEL(question), ques->question);
        gtk_label_set_text(GTK_LABEL(mainuser_A), ques->answer1);
        gtk_label_set_text(GTK_LABEL(mainuser_B), ques->answer2);
        gtk_label_set_text(GTK_LABEL(mainuser_C), ques->answer3);
        gtk_label_set_text(GTK_LABEL(mainuser_D), ques->answer4);
        g_signal_connect(GameView, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        g_object_unref(builder);
        gtk_widget_show(GameView);
        gtk_widget_hide(TopicChoice);
    }
}

void chooseTopicNormal(GtkButton *easy, gpointer data)
{
    strcpy(buff, "TOPIC NORMAL");
    request = (Request *)malloc(sizeof(Request));
    response = (Response *)malloc(sizeof(Response));
    ques = (Question *)malloc(sizeof(Question));
    setOpcodeRequest(request, buff);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    status = response->status;
    if (status == PLAYING)
    {
        strcpy(topic, response->data);
        requestGet(client_sock);
        receiveQuestion(client_sock, ques, sizeof(Question), 0);
        existQuestion = TRUE;
        questionNumber++;
    }
    if (existQuestion == TRUE)
    {
        GtkBuilder *builder;
        GtkWidget *question, *mainuser_A, *mainuser_B, *mainuser_C, *mainuser_D;
        builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
        GameView = GTK_WIDGET(gtk_builder_get_object(builder, "GameView"));
        gtk_builder_connect_signals(builder, NULL);
        question = GTK_WIDGET(gtk_builder_get_object(builder, "question"));
        mainuser_A = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_A"));
        mainuser_B = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_B"));
        mainuser_C = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_C"));
        mainuser_D = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_D"));
        score_show = GTK_WIDGET(gtk_builder_get_object(builder, "score_show"));
        survival = GTK_WIDGET(gtk_builder_get_object(builder, "survival"));
        if (ten == 0)
        {
            gtk_label_set_text(GTK_LABEL(score_show), "0");
            gtk_label_set_text(GTK_LABEL(survival), "2");
            ten = 1;
        }
        else
        {
            gcvt(infor->score, 6, scoreShow);
            sprintf(survivalShow, "%d", infor->playerPlaying);
            gtk_label_set_text(GTK_LABEL(score_show), scoreShow);
            gtk_label_set_text(GTK_LABEL(survival), survivalShow);
        }

        gtk_label_set_text(GTK_LABEL(question), ques->question);
        gtk_label_set_text(GTK_LABEL(mainuser_A), ques->answer1);
        gtk_label_set_text(GTK_LABEL(mainuser_B), ques->answer2);
        gtk_label_set_text(GTK_LABEL(mainuser_C), ques->answer3);
        gtk_label_set_text(GTK_LABEL(mainuser_D), ques->answer4);
        g_signal_connect(GameView, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        g_object_unref(builder);
        gtk_widget_show(GameView);
        gtk_widget_hide(TopicChoice);
    }
}

void chooseTopicHard(GtkButton *easy, gpointer data)
{
    strcpy(buff, "TOPIC HARD");
    request = (Request *)malloc(sizeof(Request));
    response = (Response *)malloc(sizeof(Response));
    ques = (Question *)malloc(sizeof(Question));
    setOpcodeRequest(request, buff);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    status = response->status;
    if (status == PLAYING)
    {
        strcpy(topic, response->data);
        requestGet(client_sock);
        receiveQuestion(client_sock, ques, sizeof(Question), 0);
        existQuestion = TRUE;
        questionNumber++;
    }
    if (existQuestion == TRUE)
    {
        GtkBuilder *builder;
        GtkWidget *question, *mainuser_A, *mainuser_B, *mainuser_C, *mainuser_D;
        builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
        GameView = GTK_WIDGET(gtk_builder_get_object(builder, "GameView"));
        gtk_builder_connect_signals(builder, NULL);
        question = GTK_WIDGET(gtk_builder_get_object(builder, "question"));
        mainuser_A = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_A"));
        mainuser_B = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_B"));
        mainuser_C = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_C"));
        mainuser_D = GTK_WIDGET(gtk_builder_get_object(builder, "mainuser_D"));
        score_show = GTK_WIDGET(gtk_builder_get_object(builder, "score_show"));
        survival = GTK_WIDGET(gtk_builder_get_object(builder, "survival"));
        if (ten == 0)
        {
            gtk_label_set_text(GTK_LABEL(score_show), "0");
            gtk_label_set_text(GTK_LABEL(survival), "2");
            ten = 1;
        }
        else
        {
            gcvt(infor->score, 6, scoreShow);
            sprintf(survivalShow, "%d", infor->playerPlaying);
            gtk_label_set_text(GTK_LABEL(score_show), scoreShow);
            gtk_label_set_text(GTK_LABEL(survival), survivalShow);
        }
        gtk_label_set_text(GTK_LABEL(question), ques->question);
        gtk_label_set_text(GTK_LABEL(mainuser_A), ques->answer1);
        gtk_label_set_text(GTK_LABEL(mainuser_B), ques->answer2);
        gtk_label_set_text(GTK_LABEL(mainuser_C), ques->answer3);
        gtk_label_set_text(GTK_LABEL(mainuser_D), ques->answer4);
        g_signal_connect(GameView, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        g_object_unref(builder);
        gtk_widget_show(GameView);
        gtk_widget_hide(TopicChoice);
    }
}

void clickedHelp(GtkButton *helper)
{
    requestGetHelp(client_sock);
    clickhelp_button += 1;
    receiveResponse(client_sock, response, sizeof(Response), 0);
    status = response->status;
    GtkBuilder *builder;
    builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");
    TopicChoice = GTK_WIDGET(gtk_builder_get_object(builder, "TopicChoice"));
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(TopicChoice, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_object_unref(builder);
    if (status == WAITING_QUESTION)
    {
        existQuestion = FALSE;
    }
    if (response->code == USER_USED_HINT_SUCCESS)
    {
        help = TRUE;
    }
    if (clickhelp_button == 3)
    {
        gtk_widget_hide(helper);
        requestCheckInformation(client_sock);
        receiveInformation(client_sock, infor, sizeof(Information), 0);
        if (infor->status == TRUE)
        {
            information = TRUE;
            requestGet(client_sock);
            receiveResponse(client_sock, response, sizeof(Response), 0);
            information = FALSE;
            if (response->status == END_GAME)
            {
                status = response->status;
            }
            else
            {
                gtk_widget_show(TopicChoice);
            }
        }
        else
        {
            gtk_widget_show(waiting_window);
        }
        gtk_widget_hide(GameView);
    }
    else if (clickhelp_button < 3)
    {
        requestCheckInformation(client_sock);
        receiveInformation(client_sock, infor, sizeof(Information), 0);
        if (infor->status == TRUE)
        {
            information = TRUE;
            requestGet(client_sock);
            receiveResponse(client_sock, response, sizeof(Response), 0);
            information = FALSE;
            if (response->status == END_GAME)
            {
                status = response->status;
            }
            else
            {
                gtk_widget_show(TopicChoice);
            }
        }
        else
        {
            gtk_widget_show(waiting_window);
        }
        gtk_widget_hide(GameView);
    }
}

void chooseAnswerA(GtkButton *ans_)
{
    strcpy(buff1, "ANSWER A");
    setOpcodeRequest(request, buff1);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    status = response->status;
    if (status == WAITING_QUESTION)
    {
        existQuestion = FALSE;
    }
    if (lucky == TRUE)
    {
        requestCheckInformation(client_sock);
        receiveInformation(client_sock, infor, sizeof(Information), 0);
        if (infor->status == TRUE)
        {
            information = TRUE;
            requestGet(client_sock);
            receiveResponse(client_sock, response, sizeof(Response), 0);
            information = FALSE;
            if (response->status == END_GAME)
            {
                status = response->status;
            }
            else
            {
                gtk_widget_show(TopicChoice);
            }
        }
        else
        {
            gtk_widget_show(waiting_window);
        }
        gtk_widget_hide(GameView);
    }
    else
    {
        gtk_widget_hide(GameView1);
        gtk_widget_show(waiting_window);
    }
}
void chooseAnswerB(GtkButton *ans_)
{
    strcpy(buff1, "ANSWER B");
    setOpcodeRequest(request, buff1);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    status = response->status;
    if (status == WAITING_QUESTION)
    {
        existQuestion = FALSE;
    }
    if (lucky == TRUE)
    {
        requestCheckInformation(client_sock);
        receiveInformation(client_sock, infor, sizeof(Information), 0);
        if (infor->status == TRUE)
        {
            information = TRUE;
            requestGet(client_sock);
            receiveResponse(client_sock, response, sizeof(Response), 0);
            information = FALSE;
            if (response->status == END_GAME)
            {
                status = response->status;
            }
            else
            {
                gtk_widget_show(TopicChoice);
            }
        }
        else
        {
            gtk_widget_show(waiting_window);
        }
        gtk_widget_hide(GameView);
    }
    else
    {
        gtk_widget_hide(GameView1);
        gtk_widget_show(waiting_window);
    }
}
void chooseAnswerC(GtkButton *ans_)
{
    strcpy(buff1, "ANSWER C");
    setOpcodeRequest(request, buff1);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    status = response->status;
    if (status == WAITING_QUESTION)
    {
        existQuestion = FALSE;
    }
    if (lucky == TRUE)
    {
        requestCheckInformation(client_sock);
        receiveInformation(client_sock, infor, sizeof(Information), 0);
        if (infor->status == TRUE)
        {
            information = TRUE;
            requestGet(client_sock);
            receiveResponse(client_sock, response, sizeof(Response), 0);
            information = FALSE;
            if (response->status == END_GAME)
            {
                status = response->status;
            }
            else
            {
                gtk_widget_show(TopicChoice);
            }
        }
        else
        {
            gtk_widget_show(waiting_window);
        }
        gtk_widget_hide(GameView);
    }
    else
    {
        gtk_widget_hide(GameView1);
        gtk_widget_show(waiting_window);
    }
}
void chooseAnswerD(GtkButton *ans_)
{
    strcpy(buff1, "ANSWER D");
    setOpcodeRequest(request, buff1);
    sendRequest(client_sock, request, sizeof(Request), 0);
    receiveResponse(client_sock, response, sizeof(Response), 0);
    status = response->status;
    if (status == WAITING_QUESTION)
    {
        existQuestion = FALSE;
    }
    if (lucky == TRUE)
    {
        requestCheckInformation(client_sock);
        receiveInformation(client_sock, infor, sizeof(Information), 0);
        if (infor->status == TRUE)
        {
            information = TRUE;
            requestGet(client_sock);
            receiveResponse(client_sock, response, sizeof(Response), 0);
            information = FALSE;
            if (response->status == END_GAME)
            {
                status = response->status;
            }
            else
            {
                gtk_widget_show(TopicChoice);
            }
        }
        else
        {
            gtk_widget_show(waiting_window);
        }
        gtk_widget_hide(GameView);
    }
    else
    {
        gtk_widget_hide(GameView1);
        gtk_widget_show(waiting_window);
    }
}
int main(int argc, char const *argv[])
{
    char username[BUFF_SIZE], topic[BUFF_SIZE];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, bytes_sent, bytes_received;
    char buff[BUFF_SIZE], code[BUFF_SIZE], data[BUFF_SIZE];
    request = (Request *)malloc(sizeof(Request));
    response = (Response *)malloc(sizeof(Response));
    Information *infor = (Information *)malloc(sizeof(Information));
    int lucky = FALSE, help = FALSE;

    float score = 0;
    int gameStatus = GAME_PLAYING;

    // gtk
    GtkBuilder *builder;

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

            builder = gtk_builder_new_from_file("/home/hoang/dau-truong-100/View.glade");

            index_ = GTK_WIDGET(gtk_builder_get_object(builder, "Index"));
            gtk_builder_connect_signals(builder, NULL);

            g_signal_connect(index_, "destroy", G_CALLBACK(gtk_main_quit), NULL);

            g_object_unref(builder);

            gtk_widget_show(index_);
            gtk_main();
        }
    }
}
