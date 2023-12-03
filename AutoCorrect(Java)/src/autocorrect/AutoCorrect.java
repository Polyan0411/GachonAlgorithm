package autocorrect;

import java.io.*;
import java.util.*;

public class AutoCorrect {

    // Method to calculate the edit distance between two words using dynamic programming
    public static int calculateEditDistance(String word1, String word2) {
        int len1 = word1.length();
        int len2 = word2.length();

        int[][] dp = new int[len1 + 1][len2 + 1];

        // Initializing the matrix for base cases
        for (int i = 0; i <= len1; i++) {
            dp[i][0] = i;
        }

        for (int j = 0; j <= len2; j++) {
            dp[0][j] = j;
        }

        // Filling in the matrix with edit distances
        for (int i = 1; i <= len1; i++) {
            for (int j = 1; j <= len2; j++) {
                int cost = (word1.charAt(i - 1) == word2.charAt(j - 1)) ? 0 : 1;

                dp[i][j] = Math.min(Math.min(dp[i - 1][j] + 1, dp[i][j - 1] + 1), dp[i - 1][j - 1] + cost);
            }
        }

        return dp[len1][len2];
    }

    // Method to load words from a dictionary file into a list
    public static List<String> loadWordsFromDictionary() {
        List<String> dictionaryWords = new ArrayList<>();
        String line;

        try (BufferedReader reader = new BufferedReader(new FileReader("dictionary.txt"))) {
            while ((line = reader.readLine()) != null) {
                dictionaryWords.add(line.trim().toLowerCase());
            }
        } catch (IOException e) {
            System.out.println("Error occurred while loading the dictionary.");
            e.printStackTrace();
            System.exit(1);  // Exit the program if the dictionary can't be loaded
        }

        return dictionaryWords;
    }

 // Main method to run the autocorrect program
    public static void main(String[] args) {
        List<String> dictionaryWords = loadWordsFromDictionary();
        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("Enter a word (type 'exit' to end the program):");
            String inputWord = scanner.nextLine();

            if (inputWord.equalsIgnoreCase("exit")) {
                break; // Exit the loop and end the program
            }

            // Mapping edit distances to lists of words with that distance
            Map<Integer, List<String>> distanceMap = new TreeMap<>();
            for (String word : dictionaryWords) {
                int distance = calculateEditDistance(inputWord, word);
                distanceMap.computeIfAbsent(distance, k -> new ArrayList<>()).add(word);
            }

            // Selecting top 3 suggestions based on edit distance
            List<String> suggestions = new ArrayList<>();
            for (List<String> words : distanceMap.values()) {
                suggestions.addAll(words);
                if (suggestions.size() >= 3) {
                    break;
                }
            }

            // Displaying suggestions
            System.out.println("Suggestions:");
            int numSuggestions = Math.min(3, suggestions.size());
            for (int i = 0; i < numSuggestions; i++) {
                System.out.println((i + 1) + ". " + suggestions.get(i));
            }

            // Asking user to choose a suggestion
            System.out.println("Change to:");
            int chosenNumber = -1;
            while (chosenNumber < 1 || chosenNumber > numSuggestions) {
                try {
                    chosenNumber = scanner.nextInt();
                } catch (InputMismatchException e) {
                    System.out.println("Please enter a number.");
                    scanner.next();  // Discard the invalid input
                }
            }
            scanner.nextLine(); // Consume the newline character left in the buffer
            System.out.println("AutoCorrected to: " + suggestions.get(chosenNumber - 1));
        }

        scanner.close();
    }

}
