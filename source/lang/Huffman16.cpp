#include <lang/Huffman16.h>
#include <lang/Debug.h>
#include <lang/algorithm/sort.h>
#include <limits.h>
#include <stdio.h>
#include <config.h>


#define NUM_CHARS (1<<16)
#define USE_CANONICAL_CODES
#define ENABLE_STATS


BEGIN_NAMESPACE(lang) 


Huffman16::HuffmanNode16::HuffmanNode16() :
	left( 0 ),
	right( 0 ),
	parent( 0 ),
	value( -1 ),
	code( -1 ),
	count( 0 ),
	level( 0 ),
	depth( 0 ),
	ignore( true )
{
}

Huffman16::HuffmanNode16::HuffmanNode16( HuffmanNode16* left0, HuffmanNode16* right0 ) :
	left( left0 ),
	right( right0 ),
	parent( 0 ),
	value( -1 ),
	code( -1 ),
	count( left0->count + right0->count ),
	level( 0 ),
	depth( 1+(left0->depth > right0->depth ? left0->depth : right0->depth) ),
	ignore( false )
{
	left0->parent = this;
	left0->ignore = true;
	right0->parent = this;
	right0->ignore = true;

	increaseLevel( left0 );
	increaseLevel( right0 );
}

Huffman16::Huffman16() :
	m_huffmanTree( 0 )
{
}

void Huffman16::buildTree( const uint16_t* data, int count )
{
	deleteTree();

	m_huffmanArray.resize( NUM_CHARS );
	for ( int i = 0 ; i < count ; ++i )
	{
		int ix = data[i];
		if ( m_huffmanArray[ix] == 0 )
		{
			m_allocNodesPtr.add( new HuffmanNode16() );
			m_huffmanArray[ix] = m_allocNodesPtr.last();

			m_huffmanArray[ix]->ignore = false;
			m_huffmanArray[ix]->value = ix;
		}
		m_huffmanArray[ix]->count += 1;
	}

	/*for ( int i = 0 ; i < NUM_CHARS ; ++i ) 
	{
		if ( m_huffmanArray[i] ) 
		{
			for ( int j = i+1 ; j < NUM_CHARS ; ++j ) 
			{
				if ( m_huffmanArray[j] )
				{
					if ( m_huffmanArray[i]->count < m_huffmanArray[j]->count )
					{
						HuffmanNode16* tmp = m_huffmanArray[j];
						m_huffmanArray[j] = m_huffmanArray[i];
						m_huffmanArray[i] = tmp;
					}
				}
			}
		}
	}*/
	
	/*Debug::printf( "Codes:\n" );
	int totalcount = 0;
	for ( int i = 0 ; i < NUM_CHARS ; ++i )
	{
		if ( m_huffmanArray[i] != 0 )
		{
			char sz[2] = { (char)m_huffmanArray[i]->value, 0 };
			Debug::printf( "[%d]: %d (%s) count %d\n", i, m_huffmanArray[i]->value, sz, m_huffmanArray[i]->count );
			totalcount += m_huffmanArray[i]->count;
		}
	}
	assert( totalcount == count );*/

	m_huffmanTree = buildTree( &m_huffmanArray[0], m_huffmanArray.size() );
	getLeafNodes( m_huffmanTree, m_huffmanArray );
	//printCodes( m_huffmanArray );
	//printTree( m_huffmanTree );
	assignCodes( m_huffmanArray );
#ifdef _DEBUG
	checkTree( m_huffmanTree );
#endif

	// rebuild tree to canonical
	//printCodes( m_huffmanArray );
	Array<uint8_t> tmp;
	writeTree( &tmp );
	readTree( &tmp[0], tmp.size() );
	//printCodes( m_huffmanArray );

	//Debug::printf( "Codes after tree building:\n" );
	//printCodes( m_huffmanArray );
	//Debug::printf( "Tree after tree building:\n" );
	//printTree( m_huffmanTree );

/*
	headersize = 0;
	for ( int i = 0 ; i < 0x10000 ; )
	{
		int value = i;
		int incr = 1;
		while ( i+incr < 0x10000 && lin[i+incr] == 0 && incr < (1<<12) )
			++incr;

		assert( lin[i] == 0 || lin[i]->level < 16 );
		headersize += 2;
		
		i += incr;
	}
*/
	
	/*getLeafNodes( m_huffmanTree, m_huffmanArray );
	for ( int i = 0 ; i < m_huffmanArray.size() ; ++i )
		m_huffmanArray[i]->sortvalue = -m_huffmanArray[i]->count;
	LANG_SORT( m_huffmanArray.begin(), m_huffmanArray.end(), HuffmanNodeSorter16() );
	for ( int i = 0 ; i < m_huffmanArray.size() ; ++i )
	{
		char buf[64];
		getBitString(m_huffmanArray[i]->code, m_huffmanArray[i]->level, buf);
		Debug::printf( "%c | %2d | %-6s\n", (char)m_huffmanArray[i]->value, m_huffmanArray[i]->count, buf );
	}

	int bitcount = 0;
	for ( int i = 0 ; i < count ; ++i )
	{
		int ix = -1;
		for ( int k = 0 ; k < m_huffmanArray.size() ; ++k )
		{
			if ( m_huffmanArray[k]->value == data[i] )
			{
				ix = k;
				break;
			}
		}
		
		if ( ix != -1 )
		{
			HuffmanNode16* node = m_huffmanArray[ix];
			bitcount += node->level;
		}
	}
	Debug::printf( "Total msg bits: %d\n", bitcount );
	bitcount = bitcount;*/
}

Huffman16::~Huffman16()
{
	deleteTree();
}

void Huffman16::compress( const uint16_t* data, int count, Array<uint8_t>* out )
{
	out->clear();

	// Huffman-tree
	buildTree( data, count );
	writeTree( out );

	// data size
	getLinearTableByValue( m_huffmanArray, m_encodingTable );
	int bitcount = 0;
	for ( int i = 0 ; i < count ; ++i )
	{
		HuffmanNode16* node = m_encodingTable[ data[i] ];
		assert( node != 0 );
		bitcount += node->level;
	}
	writeInt32( out, bitcount );
#ifdef _DEBUG
	int bytecount = (bitcount+7) / 8;;
	int headersize = out->size();
#endif

	// data
	encode( data, count, out );

	assert( headersize+bytecount == out->size() );
}

void Huffman16::decompress( const uint8_t* data, int count, Array<uint16_t>* out )
{
	const uint8_t* readptr = data;
	out->clear();

	// Huffman-tree
	int headersize = readTree( readptr, count );
	readptr += headersize;

	// data size
	int bitcount = readInt32( readptr );
	headersize += 4;

	// data
	decode( readptr, bitcount, out );
	readptr += (bitcount+7) >> 3;

	assert( readptr-data == count );
}

bool Huffman16::encode( const uint16_t* data, int bytecount, Array<uint8_t>* out )
{
	assert( m_huffmanTree );

	if ( 0 == m_encodingTable.size() )
		getLinearTableByValue( m_huffmanArray, m_encodingTable );

	// data -> encoded bits
	int bitcount = 0;
	Array<uint8_t> bits;
	for ( int i = 0 ; i < bytecount ; ++i )
	{
		HuffmanNode16* node = m_encodingTable[ data[i] ];
		if ( node == 0 )
			return false;

		for ( int k = 0 ; k < node->level ; ++k )
			bits.add( (node->code & (1<<k)) ? 1 : 0 );
		
		bitcount += node->level;
	}
	assert( bitcount == bits.size() );

	// DEBUG: print first 1000 bits
	/*int debugbits = 1000;
	if ( debugbits > bitcount )
		debugbits = bitcount;
	Debug::printf( "First encoded %d bits:", debugbits );
	for ( int i = 0 ; i < debugbits ; ++i )
	{
		if ( (i % 100) == 0 )
			Debug::printf( "\n" );
		Debug::printf( "%c", bits[i] ? '1' : '0' );
	}
	Debug::printf( "\n" );*/

	// encoded bits -> bytes
	for ( int i = 0 ; i < bits.size() ; i += 8 )
	{
		int b = 0;
		int c = (i+8 <= bits.size() ? 8 : bits.size()-i);
		for ( int k = 0 ; k < c ; ++k )
			b += (bits[i+k] << k);
		writeUInt8( out, b );
	}

	return true;
}

void Huffman16::decode( const uint8_t* data, int bitcount, Array<uint16_t>* out, int firstbit )
{
	assert( m_huffmanTree );

	//printCodes( m_huffmanArray );
	//Debug::printf( "Decoding %d bits", bitcount );

	HuffmanNode16* node = m_huffmanTree;
	for ( int i = firstbit ; i < bitcount ; ++i )
	{
		int bit = data[i>>3] & (1<<(i&7));
		/*if ( i < 1000 )
		{
			if ( (i%100) == 0 )
				Debug::printf( "\n" );
			Debug::printf( "%c", bit ? '1' : '0' );
		}*/

		//HuffmanNode16* prevnode = node;
		node = bit ? node->right : node->left;
		assert( node != 0 );

		if ( node->value != -1 )
		{
			out->add( uint16_t(node->value) );
			//Debug::printf( "Decoded 0x%x, restarting at %d\n", node->value, i );
			node = m_huffmanTree;
		}
	}

	//Debug::printf( "(done)\n" );
}

Huffman16::HuffmanNode16* Huffman16::buildTree( Huffman16::HuffmanNode16** nodes, int count )
{
	int min1 = -1;
	for (;;)
	{
		min1 = findMinCountNode( nodes, count );
		assert( min1 >= 0 );
		nodes[min1]->ignore = true;

		int min2 = findMinCountNode( nodes, count );
		if ( min2 < 0 )
			break;
		nodes[min2]->ignore = true;

		m_allocNodesPtr.add( new HuffmanNode16( nodes[min1], nodes[min2] ) );
		nodes[min1] = m_allocNodesPtr.last();
	}

	HuffmanNode16* rootnode = nodes[min1];
	updateDepth( rootnode );
	assignNonCanonicalCodes( rootnode );

	return rootnode;
}

int Huffman16::findMinCountNode( HuffmanNode16** nodes, int count )
{
	HuffmanNode16* minnode = 0;
	int ix = -1;

	for ( int i = 0 ; i < count ; ++i )
	{
		if ( nodes[i] != 0 && !nodes[i]->ignore )
		{
			if ( !minnode || 
				nodes[i]->count < minnode->count || 
				nodes[i]->count == minnode->count && nodes[i]->level > minnode->level )
			{
				minnode = nodes[i];
				ix = i;
			}
		}
	}

	return ix;
}

void Huffman16::printTree( HuffmanNode16* node )
{
	int spaces = node->level*4;
	for ( int i = 0 ; i < spaces ; ++i )
		Debug::printf( " " );

	char sz[8] = { (char)node->value, 0 };
	if ( node->value == -1 )
		sz[0] = '#';

	char codesz[33];
	codesz[0] = 0;
	if ( node->value != -1 )
	{
		if ( node->value == 0x20 )
		{
			node = node;
		}
		getBitString( node->code, node->level, codesz );
	}

	Debug::printf( "0x%x, count %d (code=%s)\n", node->value, node->count, codesz );
	if ( node->left )
		printTree( node->left );
	if ( node->right )
		printTree( node->right );
}

void Huffman16::increaseLevel( Huffman16::HuffmanNode16* node )
{
	if ( node )
	{
		node->level += 1;
		if ( node->left )
			increaseLevel( node->left );
		if ( node->right )
			increaseLevel( node->right );
	}
}

void Huffman16::deleteTree()
{
	while ( m_allocNodesPtr.size() > 0 )
	{
		delete m_allocNodesPtr.last();
		m_allocNodesPtr.resize( m_allocNodesPtr.size()-1 );
	}

	m_huffmanTree = 0;
}

void Huffman16::getLeafNodes( Huffman16::HuffmanNode16* node, Array<Huffman16::HuffmanNode16*>& huffmanarray )
{
	huffmanarray.clear();
	getLeafNodesR( node, huffmanarray );
}

void Huffman16::getLeafNodesR( Huffman16::HuffmanNode16* node, Array<Huffman16::HuffmanNode16*>& huffmanarray )
{
	if ( node->value != -1 )
		huffmanarray.add( node );
	if ( node->left )
		getLeafNodesR( node->left, huffmanarray );
	if ( node->right )
		getLeafNodesR( node->right, huffmanarray );
}

int Huffman16::getHuffmanCode( Huffman16::HuffmanNode16* node )
{
	int code = 0;
	int ix = 0;
	HuffmanNode16* child = node;
	int level = 0;
	for ( HuffmanNode16* parent = child->parent ; parent ; parent = parent->parent )
	{
		if ( parent->right == child )
			code += (1<<ix);
		++ix;
		child = parent;
		++level;
	}
	return reverseBits( code, level );
}

int Huffman16::reverseBits( int x, int bitcount )
{
	int x1 = 0;
	for ( int i = 0 ; i < bitcount ; ++i )
	{
		if ( x & (1<<i) )
			x1 += (1<<(bitcount-i-1));
	}
	return x1;
}

void Huffman16::assignNonCanonicalCodes( Huffman16::HuffmanNode16* node )
{
	Array<HuffmanNode16*> huffmanarray;
	getLeafNodes( node, huffmanarray );
	for ( int i = 0 ; i < huffmanarray.size() ; ++i )
	{
		HuffmanNode16* node = huffmanarray[i];
		node->code = getHuffmanCode( node );
	}
}

void Huffman16::assignCodes( Array<Huffman16::HuffmanNode16*>& huffmanarray )
{
	for ( int i = 0 ; i < huffmanarray.size() ; ++i )
		huffmanarray[i]->sortvalue = (huffmanarray[i]->level<<16) + huffmanarray[i]->value;
	LANG_SORT( huffmanarray.begin(), huffmanarray.end(), HuffmanNodeSorter16() );

	getLevelCounts( huffmanarray, m_levelCounts );

	int code = -1;
	for ( int i = 0 ; i < m_huffmanArray.size() ; ++i )
	{
		HuffmanNode16* node = m_huffmanArray[i];

		++code;
		if ( i > 0 && m_huffmanArray[i-1]->level < node->level )
			code <<= (node->level - m_huffmanArray[i-1]->level);

		node->code = reverseBits( code, node->level );
	}
}

void Huffman16::printCodes( const Array<Huffman16::HuffmanNode16*>& huffmanarray )
{
	for ( int i = 0 ; i < huffmanarray.size() ; ++i )
	{
		if ( huffmanarray[i] != 0 )
		{
			char buf[64];
			getBitString( huffmanarray[i]->code, huffmanarray[i]->level, buf );
			Debug::printf( "value=0x%4x | count=%d | level=%d | code=%8s\n", huffmanarray[i]->value, huffmanarray[i]->count, huffmanarray[i]->level, buf );
		}
	}
}

void Huffman16::getBitString( int x, int bitcount, char* buf )
{
	bool nonzero = true;
	for ( int i = bitcount-1 ; i >= 0 ; --i )
	{
		int bit = (x & (1<<i));
		if ( nonzero || bit || i == 0 )
		{
			*buf++ = ( bit ? '1' : '0' );
			nonzero = true;
		}
	}
	*buf = 0;
}

int Huffman16::getUniqueLeafLevelCount( const Array<Huffman16::HuffmanNode16*>& a )
{
	int count = 0;
	int prev = -1;
	for ( int i = 0 ; i < a.size() ; ++i )
	{
		if ( a[i]->level != prev )
		{
			assert( a[i]->level > prev );
			prev = a[i]->level;
			++count;
		}
	}
	return count;
}

void Huffman16::getLinearTableByValue( const Array<Huffman16::HuffmanNode16*>& src, 
	Array<Huffman16::HuffmanNode16*>& linear )
{
	linear.clear();
	linear.resize( 0x10000 );

	for ( int k = 0 ; k < src.size() ; ++k )
	{
		HuffmanNode16* node = src[k];
		assert( node->value >= 0 && node->value < 0x10000 );
		linear[node->value] = node;
	}
}

void Huffman16::getLinearTableByCode( const Array<Huffman16::HuffmanNode16*>& src, 
	Array<Huffman16::HuffmanNode16*>& linear )
{
	linear.clear();
	linear.resize( 0x10000 );

	for ( int k = 0 ; k < src.size() ; ++k )
	{
		HuffmanNode16* node = src[k];
		assert( node->code >= 0 && node->code < 0x10000 );
		linear[node->code] = node;
	}
}

int Huffman16::getLevelCounts( Array<Huffman16::HuffmanNode16*>& huffmanarray, Array<int>& levelcounts )
{
	levelcounts.clear();
	int uniquelevelcounts = 0;
	for ( int i = 0 ; i < huffmanarray.size() ; ++i )
	{
		HuffmanNode16* node = huffmanarray[i];
		while ( node->level >= levelcounts.size() )
			levelcounts.add( 0 );
		if ( 0 == levelcounts[node->level] )
			uniquelevelcounts++;
		levelcounts[node->level] += 1;
	}
	return uniquelevelcounts;
}

void Huffman16::writeTree( Array<uint8_t>* treedata )
{
	treedata->clear();

	int leaflevels = getLevelCounts( m_huffmanArray, m_levelCounts );
	writeUInt8( treedata, leaflevels );

	int totalcount = 0;
	int leaflevelcount = 0;
	for ( int i = 0 ; i < m_levelCounts.size() ; ++i )
	{
		if ( m_levelCounts[i] > 0 )
		{
			assert( i >= 0 && i < 256 );
			writeUInt8( treedata, i );
			writeUInt16( treedata, m_levelCounts[i] );
			totalcount += m_levelCounts[i];
			++leaflevelcount;
		}
	}
	assert( leaflevelcount == leaflevels );
	assert( totalcount == m_huffmanArray.size() );

	for ( int i = 0 ; i < m_huffmanArray.size() ; ++i )
		writeUInt16( treedata, m_huffmanArray[i]->value );
}

int Huffman16::getDepth( Huffman16::HuffmanNode16* node )
{
	int d1 = 0;
	int d2 = 0;
	if ( node->left )
		d1 = getDepth( node->left );
	if ( node->right )
		d2 = getDepth( node->right );
	return 1 + (d1 > d2 ? d1 : d2);
}

void Huffman16::updateDepth( Huffman16::HuffmanNode16* node, int level )
{
	assert( node->level == -1 || node->level == level );

	node->depth = getDepth( node );
	node->level = level;

	if ( node->left )
	{
		node->left->parent = node;
		updateDepth( node->left, level+1 );
	}
	if ( node->right )
	{
		node->right->parent = node;
		updateDepth( node->right, level+1 );
	}
}

int Huffman16::readTree( const uint8_t* treedata, int treedatasize )
{
	const uint8_t* readptr = &treedata[0];

	deleteTree();
	m_huffmanArray.clear();
	m_startCodes.clear();
	m_levelCounts.clear();
	m_encodingTable.clear();
	m_allocNodes.clear();

	int leaflevels = readUInt8( readptr );

	m_levelCounts.clear();
	int totalnodes = 0;
	for ( int i = 0 ; i < leaflevels ; ++i )
	{
		int ix = readUInt8( readptr );
		int count = readUInt16( readptr );

		if ( ix >= m_levelCounts.size() )
			m_levelCounts.resize( ix+1 );
		m_levelCounts[ix] = count;

		totalnodes += count;
	}

#ifdef ENABLE_STATS
	static int maxlevelcounts = 0;
	if ( m_levelCounts.size() > maxlevelcounts )
	{
		maxlevelcounts = m_levelCounts.size();
		Debug::printf( "maxlevelcounts = %d\n", maxlevelcounts );
	}
#endif

	int allocnodes = 1;
	while ( allocnodes < totalnodes )
		allocnodes += allocnodes;
	allocnodes += allocnodes;
	m_allocNodes.resize( allocnodes );
	int allocptr = 0;

#ifdef ENABLE_STATS
	static int maxallocnodes = 0;
	if ( allocnodes > maxallocnodes )
	{
		maxallocnodes = allocnodes;
		Debug::printf( "maxallocnodes = %d\n", maxallocnodes );
	}
#endif

	m_huffmanArray.resize( totalnodes );
	m_huffmanArray.clear();
	totalnodes = 0;
	for ( int i = 0 ; i < m_levelCounts.size() ; ++i )
	{
		for ( int k = 0 ; k < m_levelCounts[i] ; ++k )
		{
			assert( allocptr < m_allocNodes.size() );
			HuffmanNode16* node = &m_allocNodes[allocptr++];
			node->level = i;
			node->value = readUInt16( readptr );
			m_huffmanArray.add( node );
		}
		
		int levelnodecount = m_levelCounts[i];
		m_levelCounts[i] = totalnodes;
		totalnodes += levelnodecount;
	}
	assignCodes( m_huffmanArray );

	assert( m_huffmanArray.size() > 0 );
	assert( allocptr < m_allocNodes.size() );
	HuffmanNode16* root = &m_allocNodes[allocptr++];
	root->level = -1;
	for ( int i = 0 ; i < m_huffmanArray.size() ; ++i )
	{
		HuffmanNode16* node = m_huffmanArray[i];
		HuffmanNode16* parent = root;
		
		if ( node->level == 0 )
		{
			assert( m_huffmanArray.size() == 1 );
			root->left = node;
			node->level = 1;
			node->parent = root;
			break;
		}

		for ( int k = 0 ; k < node->level ; ++k )
		{
			HuffmanNode16*& next = ( node->code & (1<<k) ) ? parent->right : parent->left;
			if ( !next )
			{
				if ( k == node->level-1 )
				{
					next = node;
					break;
				}
				else
				{
					assert( allocptr < m_allocNodes.size() );
					next = &m_allocNodes[allocptr++];
					next->level = -1;
				}
			}
			assert( k+1 < node->level );
			parent = next;
		}
	}
	updateDepth( root );
	m_huffmanTree = root;

#ifdef _DEBUG
	checkTree( m_huffmanTree );
#endif

	//Debug::printf( "Codes after tree reading:\n" );
	//printCodes( m_huffmanArray );
	//Debug::printf( "Tree after tree reading:\n" );
	//printTree( root );

	int bytesread = readptr-treedata;
	assert( bytesread <= treedatasize ); treedatasize = treedatasize;
	return bytesread;
}

void Huffman16::checkTree( Huffman16::HuffmanNode16* node )
{
	if ( !node->left && !node->right )
	{
		assert( node->value != -1 );
	}
	if ( node->left )
	{
		assert( node->left->parent == node );
		checkTree( node->left );
	}
	if ( node->right )
	{
		assert( node->right->parent == node );
		checkTree( node->right );
	}
}


END_NAMESPACE() // lang
