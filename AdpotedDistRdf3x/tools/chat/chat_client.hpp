//
// chat_client.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <asio.hpp>
#include "chat_message.hpp"
#include "../rdf3xquery/rdf3xslave_query.hpp"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
using namespace std;
using asio::ip::tcp;
using asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;
#include <string>
#include <sstream>
#include <unistd.h>
#include <map>
#include<vector>

#include <chrono>

class chat_client {
private:
    map<int, vector<vector<unsigned>* >* > queriesBoarderVector;
    string peerCode;
public:
    //Ahmed : I added the extra param   char* argv[]

    chat_client(asio::io_service& io_service,
            tcp::resolver::iterator endpoint_iterator, char* thedbName, string peerCode_)
    : io_service_(io_service),
    socket_(io_service),
    dbName(thedbName),
    peerCode(peerCode_) {
        asio::async_connect(socket_, endpoint_iterator,
                boost::bind(&chat_client::handle_connect, this,
                asio::placeholders::error));
        
        tempTimeStore = 0;

    }

    void sendVector(vector<string> dataVector) {
        for (vector<string>::const_iterator iter = dataVector.begin(); iter != dataVector.end(); ++iter) {
            chat_message msg;
            string str = *iter;
            //char msg_body [msg.max_body_length - 1] = ""; 
            char t[2] = " ";
            const char *p = str.c_str();
            //             p[msg.max_body_length-2] = t[1];
            //      strcpy(msg_body , p );
            //  msg_body[msg.max_body_length-2] = t[1];
            //  int l = str.length();
            msg.body_length(msg.max_body_length - 1);
            memcpy(msg.body(), p, msg.body_length());
            msg.body()[msg.max_body_length - 2] = '\0';

            msg.encode_header();
            write(msg);
        }
    }

    void sendRegisterVector(std::vector<std::vector<unsigned>* > &regVector, int queryNr) {
        sendTempMsg();
        return;
        if(regVector.size() < 1000)
            return;
        sendTempMsg();
        cout<<"chatClient sending register size:"<<regVector.size()<<endl;
    //    string tem(dbName);
    //    if(tem.find("db1F"))
     //       return;
        int currentMsgLenght = 0;
        int msgCount = 0;
        boost::shared_ptr<string> chatMsgStr(new string);
        for (vector<vector<unsigned>* >::iterator iter = regVector.begin(); iter != regVector.end(); ++iter) {
            std::string strRow("");

            for (vector<unsigned>::iterator iter2 = (*iter)->begin(); iter2 != (*iter)->end(); ++iter2) {
                std::string str = to_string((*iter2));
                //strRow.append(str.c_str());
                strRow.append(str);
                strRow.append(",");

            }

            size_t l = strRow.length();
            if (currentMsgLenght + l + 14 < chat_message::max_body_length) {
                chatMsgStr->append(strRow);
                chatMsgStr->append("-");
                currentMsgLenght += l;
            } else {
                setRegMsgHeader(chatMsgStr, queryNr, msgCount);
                writeChatMsg(chatMsgStr);
                msgCount++;
                currentMsgLenght = l;
                chatMsgStr = boost::make_shared<std::string>(strRow);
                chatMsgStr->append("-");
            }
            // cout<<endl;

            //cout <<strRow<<endl;
        }
        if (regVector.size() == 0) { // we have nothing to send for this query , just inform the others
            chatMsgStr->append("no");
            setRegMsgHeader(chatMsgStr, queryNr, 0);
        } else {
            //write the last string
            if (chatMsgStr->find_first_of('*') == string::npos)
                setRegMsgHeader(chatMsgStr, queryNr, msgCount);
            chatMsgStr->append("f");
            if (chatMsgStr->find_first_of('f') != string::npos) {
                // cout<<"contain :"<<*chatMsgStr<<endl;
            }
        }

        writeChatMsg(chatMsgStr);



    }

    void sendRegisterVector(std::vector<std::vector<Register*>* > regVector, int queryNr) {
       
        int currentMsgLenght = 0;
        int msgCount = 0;
        boost::shared_ptr<string> chatMsgStr(new string);
        for (vector<vector<Register*>* >::iterator iter = regVector.begin(); iter != regVector.end(); ++iter) {
            std::string strRow("");

            for (vector<Register*>::iterator iter2 = (*iter)->begin(); iter2 != (*iter)->end(); ++iter2) {
                std::string str = to_string((*iter2)->value);
                //strRow.append(str.c_str());
                strRow.append(str);
                strRow.append(",");

            }

            size_t l = strRow.length();
            if (currentMsgLenght + l + 14 < chat_message::max_body_length) {
                chatMsgStr->append(strRow);
                chatMsgStr->append("-");
                currentMsgLenght += l;
            } else {
                setRegMsgHeader(chatMsgStr, queryNr, msgCount);
                writeChatMsg(chatMsgStr);
                msgCount++;
                currentMsgLenght = l;
                chatMsgStr = boost::make_shared<std::string>(strRow);
                chatMsgStr->append("-");
            }
            // cout<<endl;

            //cout <<strRow<<endl;
        }
        //write the last string
        if (chatMsgStr->find_first_of('*') == string::npos)
            setRegMsgHeader(chatMsgStr, queryNr, msgCount);
        chatMsgStr->append("f");
        if (chatMsgStr->find_first_of('f') != string::npos) {
            // cout<<"contain :"<<*chatMsgStr<<endl;
        }

        writeChatMsg(chatMsgStr);

    }
    
    void sendTempMsg(){
        
        boost::shared_ptr<string> chatMsgStr(new string);
         srand(time(NULL));
         int n ;
        for(int i= 0 ; i < 20000 ; i++){
            n = rand()%1000 +1;
            chatMsgStr->append(to_string(n));
        }
         tempTimeStore = clock();
         writeChatMsg(chatMsgStr);
    }

    void setRegMsgHeader(boost::shared_ptr<string> str, int queryNr, int msgCount) {
        string startSequence = "*";
        startSequence.append(to_string(queryNr));
        startSequence.append("#");
        startSequence.append(to_string(msgCount));
        startSequence.append(",");
        startSequence.append(peerCode);
        startSequence.append("$");
        str->insert(0, startSequence);
    }

    void writeChatMsg(boost::shared_ptr<string> msgLine) {
        chat_message msg;
        char line[chat_message::max_body_length + 1];
        msg.body_length(msg.max_body_length - 1);
        memcpy(msg.body(), msgLine->c_str(), msg.body_length());
        msg.body()[msg.max_body_length - 1] = '\0';
        // msg.body_length(strlen(line));
        //memcpy(msg.body(), line, msg.body_length());
        msg.encode_header();
        write(msg);
    }

    string to_string(const unsigned& n) {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }

    void write(const chat_message& msg) {
        io_service_.post(boost::bind(&chat_client::do_write, this, msg));
    }

    void close() {
        io_service_.post(boost::bind(&chat_client::do_close, this));
    }

private:

    void handle_connect(const asio::error_code& error) {
        if (!error) {
            asio::async_read(socket_,
                    asio::buffer(read_msg_.data(), chat_message::header_length),
                    boost::bind(&chat_client::handle_read_header, this,
                    asio::placeholders::error));
        }
    }

    void handle_read_header(const asio::error_code& error) {
        if (!error && read_msg_.decode_header()) {
            asio::async_read(socket_,
                    asio::buffer(read_msg_.body(), read_msg_.body_length()),
                    boost::bind(&chat_client::handle_read_body, this,
                    asio::placeholders::error));
        } else {
            do_close();
        }
    }
    clock_t tempTimeStore ; 
    void handle_read_body(const asio::error_code& error) {
        if (!error) {
            // std::cout.write(read_msg_.body(), read_msg_.body_length());
            //  std::cout << "\n";
            asio::async_read(socket_,
                    asio::buffer(read_msg_.data(), chat_message::header_length),
                    boost::bind(&chat_client::handle_read_header, this,
                    asio::placeholders::error));

            
            ////temp code please remove
            if(tempTimeStore != 0){
                string st = read_msg_.body();
                cout<<" temp time "<<(clock() - tempTimeStore) <<"size of data:"<<st.length()<< endl;
            }
            /////
            
            char* argv[] = {"any", "db"};
            argv[1] = dbName;
            string query = read_msg_.body();
            //          cout << "(" << query.substr(0, read_msg_.body_length()) << ")" << endl;
            //          cout<<"message body:"<<query<<endl;
            if (query.at(0) == '#') {
                size_t i = query.find_first_of('*');
                string queryCodeStr = query.substr(1, i - 1);
                int queryCode = atoi(queryCodeStr.c_str());
                string queryPure = query.substr(i + 1);
                cout << "executing .. " << queryPure << endl;
                RDF3xQuery* queryInterface = new RDF3xQuery(2);
                queryInterfaceMap[queryCode] = queryInterface;
                peersResponseCount[queryCode] = 0;
                db = new Database();
                queryStartTime[queryCode] = clock();
                queryInterface->exeQuery(2, argv, queryPure, *db, this, queryCode);

            }
            if (query.at(0) == '*') {
                // queryInterfaceMap;
                //its vector message!
                size_t i = query.find_first_of('#');
                string queryCodeStr = query.substr(1, i);
                int queryCode = atoi(queryCodeStr.c_str());
                size_t j = query.find_first_of('$');
                string msgNoStr = query.substr(i + 1, j - i - 1);
                size_t commaIndex = msgNoStr.find_first_of(',');
                string msgNoStrF = msgNoStr.substr(0, commaIndex + 1);
                string senderPeerCode = msgNoStr.substr(commaIndex + 1);
                int msgNo = atoi(msgNoStrF.c_str());
                string regStr = query.substr(j + 1);
                if (senderPeerCode.compare(peerCode) != 0) { // only if the message not from us
                    if (regStr.compare("no") == 0)
                        processQueryBoarder(queryCode, false);
                    bool final = false;
                    
                    getRegisterVector(regStr, queryCode, final);
                    // cout<<regStr<<endl;
                    if (final){
                        sendAknoldegmnet(queryCode);
                        processQueryBoarder(queryCode, true);
                        
                    }
                }

            }
            if(query.at(0) == 'A'){
                string queryCodeStr = query.substr(4);
                size_t i = query.find_first_of('^');
                string recDBName = query.substr(i+1);
                cout<<recDBName<<endl;
                if(recDBName.compare(dbName) != 0)
                    gotAcknlodegment(queryCodeStr);
            }

            //unsigned int microseconds = 10000000;
            //usleep(microseconds);

        } else {
            do_close();
        }
    }
    
    void gotAcknlodegment(string queryCode){
        cout<<"time to send vector :"<< clock() - tempTimeStore<<endl;
    }
    
    
    
    void sendAknoldegmnet(int queryCode){
        boost::shared_ptr<string> chatAckStr(new string);
        std::string queryCodeStr = to_string(queryCode);
        chatAckStr->append("ACK-");
        chatAckStr->append(queryCodeStr);
        
        chatAckStr->append("^");
         chatAckStr->append(dbName);
         cout<<"sending acknoldgment"<<endl;
        writeChatMsg(chatAckStr);
    }

    void processQueryBoarder(int queryCode, bool responseOk) {
        vector<vector<unsigned> > boarderBuffer;
        vector<vector<unsigned>* >* vv = queriesBoarderVector[queryCode];
        RDF3xQuery* queryInterface = queryInterfaceMap[queryCode];
        peersResponseCount[queryCode]++;
        if (queryInterface == 0) {
            cout << "error finding query interface" << endl;
            exit(1);
        }
        if (responseOk)
            queryInterface->workFinalBoarder(vv, queryCode);
        if (peersResponseCount[queryCode] == queryInterface->peersNo-1) {
            clock_t start = queryStartTime[queryCode];
            clock_t end = clock();
            cout << "query number:" << queryCode << " is done , time:"<<(end - start) <<"clocks "<<(end - start)/CLOCKS_PER_SEC <<"seconds" <<endl<<"clocks per seconds: "<<CLOCKS_PER_SEC <<endl;

        }

    }

    vector<vector<unsigned>* >* getRegisterVector(string regStr, int queryCode, bool &finalMsg) {
        vector<vector<unsigned>* >* res = queriesBoarderVector[queryCode];
        if (res == 0) {
            ///vector<unsigned>* innerVec = new vector<unsigned>*();
            res = new vector<vector<unsigned>* >();
            queriesBoarderVector[queryCode] = res;

        }
        vector<string> strings;
        istringstream f(regStr);
        string s;
        bool final = false;
        if (regStr.find_first_of('f') != string::npos)
            final = true;
        while (getline(f, s, '-')) {
            if (s.length() > 0) {
                size_t fi_index = s.find_first_of('f');
                if (fi_index != string::npos) {
                    s = s.substr(0, fi_index);
                    final = true;
                }
                strings.push_back(s);
            }
        }
        for (int i = 0; i < strings.size(); i++) {
            istringstream f(strings[i]);
            vector<unsigned> *reg = new vector<unsigned>();
            while (getline(f, s, ',')) {
                if (s.length() > 0) {
                    unsigned r = (unsigned) atoi(s.c_str());
                    reg->push_back(r);
                }
            }
            res->push_back(reg);
        }
        finalMsg = final;
        return res;
    }

    void do_write(chat_message msg) {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            asio::async_write(socket_,
                    asio::buffer(write_msgs_.front().data(),
                    write_msgs_.front().length()),
                    boost::bind(&chat_client::handle_write, this,
                    asio::placeholders::error));
        }
    }

    void handle_write(const asio::error_code& error) {
        if (!error) {
            write_msgs_.pop_front();
            if (!write_msgs_.empty()) {
                asio::async_write(socket_,
                        asio::buffer(write_msgs_.front().data(),
                        write_msgs_.front().length()),
                        boost::bind(&chat_client::handle_write, this,
                        asio::placeholders::error));
            }
        } else {
            do_close();
        }
    }

    void do_close() {
        socket_.close();
    }

private:
    asio::io_service& io_service_;
    tcp::socket socket_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;

    map<int, RDF3xQuery*> queryInterfaceMap;
    map<int, int> peersResponseCount;
    map<int, clock_t> queryStartTime;
    Database *db;
    char* dbName;

};



#endif
