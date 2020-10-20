//
//  inodemap.c
//  mytar
//
//  Created by Dorian Arnold on 10/8/20.
//  Copyright © 2020 Dorian Arnold. All rights reserved.
//

#include "inodemap.h"
#include <stdio.h>
#include <stdlib.h>

char const ** Map=NULL;

void set_inode( ino_t i, const char * f ){
    u_int32_t mappos = i % MAPSIZE;
    
    if( Map == NULL )
        Map = (const char  **)calloc(MAPSIZE, sizeof(char*));
    
    Map[mappos] = f;
}

const char * get_inode( ino_t i ){
    if( Map == NULL )
        Map = (const char  **)calloc(MAPSIZE, sizeof(char*));
    
    return Map[ i % MAPSIZE ];
}
