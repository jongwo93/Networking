/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Jongwoo Jang
 */
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

import java.io.*;
import static java.lang.Math.random;
import java.net.*;
import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;
/**
 *
 * @author nick
 */
public class Server extends Thread {
    
    static String[] words = {"question", "random", "home", "lime", "game", "play", "plan", "prime",
        "plane", "quiet", "start", "stand", "airplane", "anything", "problems", "critical"};
    
    static ServerSocket serverSocket;
    static Socket clientSocket;
    
    static final int MAX_CLIENT = 3;
    static players_thread[] player = new players_thread[MAX_CLIENT];

    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception {

        int portNumber = 9595;
        String server_IP = "";
        if(args.length < 1) {
            System.out.println("Using default port : " + portNumber);
        } else {
            portNumber = Integer.valueOf(args[0]).intValue();
        }
        //Opens Server
        try {
            serverSocket = new ServerSocket(portNumber);
        } catch(Exception e) {
            System.out.println("main first : " + e);
        }
        
        while(true) {
            try {
                clientSocket = serverSocket.accept();
                int count = 0;
                for(count = 0; count < MAX_CLIENT; count++) {
                    if(player[count] == null) {
                        String word;
                        int index = (int)(Math.random() * words.length) - 1;
                        if(index == -1) {
                            index = 0;
                        }
                        word = words[index];
                        
                        (player[count] = new players_thread(clientSocket, player, word)).start();
                        break;
                    }
                }
                if(count == MAX_CLIENT) {
                    PrintStream os = new PrintStream(clientSocket.getOutputStream()); 
                    System.out.println("count : " + count);
                    os.println("Server too busy. Try later.");
                    //change to DataOutputStream
                    os.close();
                    clientSocket.close();
                }
            } catch(Exception e) {
                System.out.println("main while loop : " + e);
            }
        }
    }
    
}

class players_thread extends Thread {

    private DataInputStream inFromClient = null;
    private DataOutputStream outToClient = null;
    private Socket clientSocket;

    private players_thread[] player;
    private int MAX_CLIENT;
    private String gameWord = null;
    
    private boolean wordFound = false;
    private boolean foundChar = false;
    private int guesses = 0;
    private int lettersleft =0;
    private String input = "\0";
    private char[] wordArray = null;
    private char[] guessed = null;
    private String wrongGuess = null;
    private String dashes = null;
    
    public players_thread(Socket clientSocket, players_thread[] player, String gameWord) {
        this.clientSocket = clientSocket;
        this.player = player;
        this.gameWord = gameWord;
        MAX_CLIENT = player.length;
        initialize();
        
    }
    public void initialize() {
        this.lettersleft = gameWord.length();
        this.wordArray = gameWord.toCharArray();
        this.guessed = new char[wordArray.length];
        //put dashes for game word
        for(int i = 0; i < guessed.length; i++) {
            guessed[i] = '-';
        }
        this.guesses = 6;
        this.wrongGuess = "";
    }
    
    
    public void run() {
        int MAX_CLIENT = this.MAX_CLIENT;
        players_thread[] player = this.player;
        try {
            inFromClient = new DataInputStream(clientSocket.getInputStream());
            outToClient = new DataOutputStream(clientSocket.getOutputStream());

            String in_packet = "";
            String out_packet = "";
            System.out.println("player connected," + Thread.currentThread().toString());

            System.out.println("game word = " + gameWord);
            for(int i = 0; i < guessed.length; i++) {
                System.out.print(guessed[i]);
                dashes += guessed[i];
                
            }
            System.out.println();
            while(true) {
                byte[] bs = new byte[2];

                boolean end = false;
                int read;
                String line = "";
                while(!end) {
                    read = this.inFromClient.read(bs);
                    line += new String(bs, 0, read);
                    
                    if(line.charAt(0) == '0' || line.length() == 2) {
                        end = true;
                    }
                }
                //System.out.println("in while : " + line);

                if(line.startsWith("/quit")) {
                    break;
                }
                if(line.charAt(0) == '0') { //game started, 
                    line = "";
                    out_packet = packetize(0);
                    this.outToClient.writeBytes(out_packet);
                    this.outToClient.flush();
                } else { 
                    for(int i = 0; i < wordArray.length; i++) {
                        if(line.charAt(1) == wordArray[i]) {
                            guessed[i] = line.charAt(1);
                            foundChar = true;
                        }
                    }
                    
                    if(!foundChar) {
                        wrongGuess += line.charAt(1);
                        guesses--;
                        
                    } else {
                        lettersleft = computeLettersLeft(guessed);
                        //System.out.println(lettersleft);
                        foundChar = false;
                    }
                    
                    if(guesses == 0) {
                        out_packet = packetize(1);
                        this.outToClient.writeBytes(out_packet);
                        this.outToClient.flush();
                        break;
                    }
                    if(lettersleft == 0) {
                        out_packet = packetize(2);
                        this.outToClient.writeBytes(out_packet);
                        this.outToClient.flush();
                        break;
                    }
                    out_packet = packetize(0);
                    this.outToClient.writeBytes(out_packet);
                    this.outToClient.flush();
                    line = "";
                    
                    
                    
                }
            }
            System.out.println("Closing the connection to " + Thread.currentThread().toString());
            for(int i = 0; i < MAX_CLIENT; i++) {
                if(player[i] == this) {
                    player[i] = null;
                }
            }
            
            inFromClient.close();
            outToClient.close();
            clientSocket.close();
        } catch (Exception e) {
            System.out.println("run method : " + e);
        }
    }
    private int computeLettersLeft(char[] guessed) {
        int retVal = 0;
        for(int i = 0; i < guessed.length; i++) {
            if(guessed[i] == '-') retVal++;
        }
        return retVal;
    }

    private String packetize(int num) { /********* DOES NOT SHOW WRONGGUESSES***/
        String retVal = "";
        if(num == 0) { // normal game control message
            retVal = "0" + wordArray.length + (10 - guesses);
            
            for(int i = 0; i < guessed.length; i++) {
                retVal += guessed[i];
            }
            if(wrongGuess != null) {
                for(int i = 0; i < wrongGuess.length(); i++) {
                retVal += wrongGuess.charAt(i);
                }
            }
            
        } else if(num == 1) {
            String msg = "The word was " + gameWord + " Game Over, You Lose.";
            
            retVal = msg.length() + msg;
            
        } else if(num == 2) {
            String msg = "The word was " + gameWord + " Congrats, You won.";
            retVal = msg.length() + msg;
        }
        
        return retVal;
    }
}