package com.example.vmm.services;

import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;

@Service
public class DTWSearchUtil {


    private int[][] getDTWMatrix(int[] ts1, int[] ts2, Integer w) {
        int[][] matrix = new int[ts1.length+1][ts2.length+1];

        if (w == null) {
            w = Math.max(ts1.length, ts2.length);
        }

        w = Math.max(w, Math.abs(ts1.length - ts2.length));

        for (int i = 0; i <= ts1.length; i++) {
            for (int j = 0; j <= ts2.length; j++) {
                matrix[i][j] = Integer.MAX_VALUE;
            }
        }

        for (int i = 1; i <= ts1.length; i++) {
            for (int j = Math.max(1, i-w); j <= Math.min(ts2.length, i+w); j++) {
                matrix[i][j] = 0;
            }
        }



        matrix[0][0] = 0;

        for (int i = 1; i <= ts1.length; i++) {
            for (int j = Math.max(1, i-w); j <= Math.min(ts2.length, i+w); j++) {
                int cost = getDistance(ts1[i-1], ts2[j-1]);
                matrix[i][j] = cost + Math.min(matrix[i-1][j],Math.min(matrix[i][j-1], matrix[i-1][j-1]));
            }
        }

        return matrix;
    }

    protected int getDTWDistance(int[] ts1, int[] ts2, Integer w) {
        int[][] matrix = getDTWMatrix(ts1,ts2,w);
        return matrix[ts1.length][ts2.length];
    }


    protected Integer[][] getAlignedSequence(int[] ts1, int[] ts2, Integer w) {
        int[][] matrix = getDTWMatrix(ts1,ts2,w);

        int cordX = ts1.length-1;
        int cordY = ts2.length-1;

        List<Integer> string1 = new ArrayList<>();
        List<Integer> string2 = new ArrayList<>();
        string1.add(ts1[cordX]);
        string2.add(ts2[cordY]);

        do {
            int min = Math.min(matrix[cordX][cordY+1], Math.min(matrix[cordX][cordY], matrix[cordX+1][cordY]));

            if(min == matrix[cordX][cordY]) {
                string1.add(ts1[cordX--]);
                string2.add(ts2[cordY--]);
            } else if (min == matrix[cordX][cordY+1]) {
                string1.add(ts1[cordX--]);
                string2.add(null);
            } else if(min == matrix[cordX+1][cordY]) {
                string1.add(null);
                string2.add(ts2[cordY--]);
            }

        } while (cordX != 0 && cordY!=0);

        int outputSize =Math.max(string1.size(),string2.size());

        Integer[][] result = new Integer[2][outputSize];

        string1 = string1.reversed();
        string2 = string2.reversed();

        for (int i = 0; i < outputSize; i++) {
            result[0][i] = string1.get(i);
            result[1][i] = string2.get(i);
        }

        return result;

    }


    protected int getDistance(int x, int y) {
        return Math.abs(x-y);
    }
}
