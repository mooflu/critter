// Description:
//   Enemy factory to help create enemy objects.
//
// Copyright (C) 2001 Frank Becker
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation;  either version 2 of the License,  or (at your option) any  later
// version.
//
// This program is distributed in the hope that it will be useful,  but  WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details
//
#include <string>

#include <LevelPack.hpp>
#include <EnemyFactory.hpp>
#include <LPathManager.hpp>
#include <SimpleEnemy.hpp>
#include <Boss1Enemy.hpp>

enum EnemyType
{
    eSimpleEnemy = 0,
    eBoss1 = 1
};

//helper
inline static float strToFloat( const string &str)
{
    return (float)atof( str.c_str());
}

inline static int strToInt( const string &str)
{
    return atoi( str.c_str());
}

//count siblings of the same type
inline static int countSiblings( TiXmlNode *node, const string & name)
{
    int count = 0;
    while( node)
    {
	count++;
	node = node->NextSibling( name.c_str());
    }
    return count;
}

void EnemyFactory::createEnemy( TiXmlNode *enemyNode)
{
    TiXmlNode *node;
    TiXmlElement *elem;
    
    LPathManager &lpm = *LPathManagerS::instance();

    LEnemy *enemy = new LEnemy();

    elem = enemyNode->ToElement();
    enemy->spawnTime = strToFloat( elem->Attribute("spawnTime"));
    EnemyType enemyType = eSimpleEnemy;
    if( elem->Attribute("type"))
	enemyType = (EnemyType)strToInt(elem->Attribute("type"));

    node = enemyNode->FirstChild("Model");
    elem = node->ToElement();
    enemy->modelName = elem->Attribute("Name");
    
    node = enemyNode->FirstChild("SpawnPoint");
    elem = node->ToElement();
    enemy->spawnPoint.x = strToFloat( elem->Attribute("x"));
    enemy->spawnPoint.y = strToFloat( elem->Attribute("y"));
    enemy->spawnPoint.z = strToFloat( elem->Attribute("z"));
    
    node = enemyNode->FirstChild("Home");
    elem = node->ToElement();
    enemy->home.x = strToFloat( elem->Attribute("x"));
    enemy->home.y = strToFloat( elem->Attribute("y"));
    enemy->home.z = strToFloat( elem->Attribute("z"));

    node = enemyNode->FirstChild("EntryPath");
    elem = node->ToElement();
    enemy->entry = lpm.getPath( elem->Attribute("Name"));

    node = enemyNode->FirstChild("IdlePath");
    elem = node->ToElement();
    enemy->idle = lpm.getPath( elem->Attribute("Name"));

    node = enemyNode->FirstChild("AttackPath");
    enemy->numAttackPaths = countSiblings( node, "AttackPath");
    enemy->attack = new LPath*[ enemy->numAttackPaths];
    for( int i=0; i< enemy->numAttackPaths; i++)
    {
	elem = node->ToElement();
	enemy->attack[ i] = lpm.getPath( elem->Attribute("Name"));
	node = node->NextSibling( "AttackPath");
    }

    node = enemyNode->FirstChild("RetreatPath");
    enemy->numRetreatPaths = countSiblings( node, "RetreatPath");
    enemy->retreat = new LPath*[ enemy->numRetreatPaths];
    for( int i=0; i< enemy->numRetreatPaths; i++)
    {
	elem = node->ToElement();
	enemy->retreat[ i] = lpm.getPath( elem->Attribute("Name"));
	node = node->NextSibling( "RetreatPath");
    }

    switch( enemyType)
    {
	default:
	case eSimpleEnemy:
	    new SimpleEnemy( enemy);
	    break;
	case eBoss1:
	    new Boss1Enemy( enemy);
	    break;
    }
}
