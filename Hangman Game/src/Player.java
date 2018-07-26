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
import java.net.*;
import java.util.*;

/**
 *
 * @author nick
 */
public class Player implements Runnable {
    private static Socket clientSocket;

    private static DataOutputStream outToServer;
    private static DataInputStream inFromServer;
    private static Scanner sc;
    private static BufferedReader userInput;
    private static boolean closed = false;
    private static byte[] in_Packet = new byte[255];
    private static byte[] out_Packet = null;
    private static int wordLength = 0;
    private static int numWrong;
    private static String guessWrong = "";
    private static String guessed = "";
    private static String decodedMSG;
    private static boolean proceed = false;
    
    public static void main (String[] args) {
        
        int portNumber = 9595;
        String host = "localhost";
        
        if(args.length < 2) {
            System.out.println("Using default host : " + host + ", port : " + portNumber);
        } else {
            host = args[0];
            portNumber = Integer.valueOf(args[1]).intValue();
        }
        try {
            clientSocket = new Socket(host, portNumber);
            userInput = new BufferedReader(new InputStreamReader(System.in));
            outToServer = new DataOutputStream(clientSocket.getOutputStream());
            //os = new PrintStream(clientSocket.getOutputStream());
            inFromServer = new DataInputStream(clientSocket.getInputStream());
            sc = new Scanner(System.in);
        } catch(Exception e) {
            //System.out.println("client main first : " + e);
        }
        
        if(clientSocket != null && outToServer != null && inFromServer != null) {

            try {
                //create a thread
                new Thread(new Player()).start();
                boolean gameStart = false;
                String answer = "";
                while(!gameStart) {
                    System.out.println("Ready to start game? (y/n)");
                    answer = userInput.readLine().trim();
                
                    answer = answer.toLowerCase();
                    if(answer.equals("y") || answer.equals("n")) { 
                        gameStart = true;
                    } else {
                        System.out.println("Type 'Y' or 'N'");
                    }
                }
                if(answer.equals("y")) {

                    System.out.println();
                    String startGame = "0";
                    
                    outToServer.writeBytes(startGame);
                    outToServer.flush();

                    while(!closed) {
                        
                        while(!proceed) {
                            Thread.sleep(10);
                            int i = 0;
                            i++;
                        }
                        
                        System.out.println(decodedMSG);
                        if(closed) {
                            //System.out.println("CLOSED");
                            break;
                        }
                        System.out.print("Incorrect Guesses: ");
                        if(numWrong != 0) {
                            for(int i = 0; i < guessWrong.length(); i++) {
                                System.out.print(guessWrong.charAt(i) + " ");
                            }
                        }
                        
                        System.out.println();
                        System.out.println();
                        
                        String message = "";
                        boolean valid = false;
                        while(!valid) {
                            System.out.print("Letter to guess: ");
                            String letter = sc.nextLine();
                            letter = letter.toLowerCase();
                            
                            if(letter.matches("[a-z]+") && letter.length() == 1 && guessed.indexOf(letter) == -1) {
                                message = "1" + letter;
                                guessed += letter;
                                valid = true;
                            } else {
                                   if(guessed.indexOf(letter) != -1) {
                                       System.out.println("You've already guessed that letter.");
                                       
                                   } else { 
                                       System.out.println("Wrong Input, please guess an alphabet letter.");
                                   }
                                   System.out.println();
                            }
                        }
                        
                        outToServer.writeBytes(message);
                        outToServer.flush();
                        proceed = false;

                    }
                } else if(answer.equals("n")){
                    closed = true;
                }

                outToServer.close();
                inFromServer.close();
                clientSocket.close();
                
            } catch(Exception e) {
                //System.out.println("client main second : " + e);
            }
        }
        
    }

    //thread to read from the server
    public void run() {
        //keep reading til we get "game over" message from server
        String msgFromServer ="";
        int read = 0;
        int prev = 0;
        int prevWrong = 0;
        int readcounter = 0;
        try {

            boolean end = false;
            while(true) {
                while(!end) {
                    read = inFromServer.read(in_Packet);
                    readcounter += read;
                    msgFromServer += new String(in_Packet, 0, read);
                    if(readcounter != 1 && readcounter >= prev) {
                        prev = readcounter;
                        readcounter = 0;
                        end = true;
                    }
                }

                decodedMSG = decodePacket(msgFromServer);
                in_Packet = new byte[255];
                msgFromServer = "";
                proceed = true;
                end = false;
                /** if quit, closed = true ***/
            }
        } catch(Exception e) {
            //System.out.println("Client run : " + e);
        }
    
    }
    
    private static String decodePacket(String dec) {

        String retVal = "";
        
        if(dec.charAt(0) == '0') {
            if(wordLength == 0) {
                wordLength = Integer.parseInt(dec.substring(1,2));
            }
            numWrong = Integer.parseInt(dec.substring(2,3));
            if(numWrong != 0) {
                guessWrong = dec.substring((3+wordLength));
            }
            retVal = dec.substring(3, 3+wordLength); /***********/
        } else {

            retVal = dec.substring(2);
            closed = true;
        }
        return retVal;
    }

}
