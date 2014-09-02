#ifndef THEOREM_PROVER_FIRST_ORDER_LOGIC_DEDUCTION_TREE
#define THEOREM_PROVER_FIRST_ORDER_LOGIC_DEDUCTION_TREE
#include "predicate.hpp"
#include "memory"
#include "utility"
#include "term_generator.hpp"
#include "boost/range.hpp"
#include "boost/range/join.hpp"
#include "boost/iterator/counting_iterator.hpp"
#include "function.hpp"
#include "predicate.hpp"
#include "proof_tree.hpp"
#include "sentence.hpp"
#include "first_order_logic.hpp"
#include "substitution.hpp"
namespace first_order_logic
{
/*
	struct gentzen_system
	{
		std::shared_ptr< proof_tree > pt;
		term new_variable( )
		{
			term ret = make_variable( std::to_string( unused++ ) );
			cv_map.insert( std::make_pair( ret,  std::set< sentence >( ) ) );
			return ret;
		}
		std::map< sentence, bool > sequent;
		std::map< sentence, bool > temp_sequent;
		std::map
		<
			term,
			std::set< sentence >
		> cv_map, sentence_map;
		std::map< sentence, bool > expanded;
		size_t unused = 0;
		std::set< function > functions;
		std::set< predicate > predicates;
		term_generator< gentzen_system > tg;
		bool is_valid( std::shared_ptr< proof_tree > & pt, sentence t, bool b )
		{
			gentzen_system dt( * this );
			try { dt.try_insert( dt.sequent, t, b ); }
			catch ( contradiction & con )
			{
				join( pt, con.pt );
				return true;
			}
			auto res = dt.is_valid( );
			join( pt, dt.pt );
			return res;
		}
		struct contradiction { std::shared_ptr< proof_tree > pt; };
		void try_insert(
				std::map< sentence, bool > & m,
				const sentence & t,
				bool b )
		{
			if ( m.insert( std::make_pair( t, b ) ).first->second != b )
			{
				contradiction con;
				auto res = pair_str( );
				std::string & str = b ? res.first : res.second;
				if ( ! str.empty( ) ) { str += ","; }
				str += static_cast< std::string >( t );
				con.pt.reset( new proof_tree( res.first + "-->" + res.second ) );
				throw con;
			}
		}
		std::shared_ptr< proof_tree > join( std::shared_ptr< proof_tree > & parent, std::shared_ptr< proof_tree > child )
		{
			child->parent = parent.get( );
			if ( child->str != parent->str )
			{
				parent->child.push_back( child );
				return child;
			}
			return parent;
		}
		void add_equal_generator( const function & f )
		{
			assert( f.arity >= 1 );
			if ( f.arity == 1 )
			{
				try_insert( sequent,
							make_all(
								"s",
								make_all(
									"t",
									make_imply(
											make_equal(
												make_variable( "s" ),
												make_variable( "t" ) ),
											make_equal(
												make_function( f.name, { make_variable( "s" ) } ),
												make_function( f.name, { make_variable( "t" ) } ) ) ) ) ),
							true );
			}
			else
			{
				std::vector< term > args, argt;
				args.reserve( f.arity );
				argt.reserve( f.arity );
				std::for_each(
							boost::counting_iterator< size_t >( 0 ),
							boost::counting_iterator< size_t >( f.arity ),
							[&]( size_t i )
				{
					args.push_back( make_variable( "s" + std::to_string( i ) ) );
					argt.push_back( make_variable( "t" + std::to_string( i ) ) );
				} );
				sentence and_stack =
						make_and(
							make_equal( args[0], argt[0] ),
							make_equal( args[1], argt[1] ) );
				for ( size_t i = 2; i < f.arity; ++i ) { and_stack = make_and( and_stack, make_equal( args[i], argt[i] ) ); }
				auto add = make_imply( and_stack, make_equal( make_function( f.name, args ), make_function( f.name, argt ) ) );
				for ( size_t i = 0; i < f.arity; ++i ) { add = make_all( args[i]->name, make_all( argt[i]->name, add ) ); }
				try_insert( sequent, add, true );
			}
		}
		void add_equal_generator( const predicate & f )
		{
			assert( f.arity >= 1 );
			if ( f.arity == 1 )
			{
				try_insert( sequent,
							make_all(
										"s",
										make_all(
												"t",
												make_imply(
														make_and(
															make_equal(
																make_variable( "s" ),
																make_variable( "t" ) ),
														make_predicate( f.name, { make_variable( "s" ) } ) ),
												make_predicate( f.name, { make_variable( "t" ) } ) ) ) ),
										true );
			}
			else
			{
				std::vector< term > args, argt;
				args.reserve( f.arity );
				argt.reserve( f.arity );
				std::for_each(
							boost::counting_iterator< size_t >( 0 ),
							boost::counting_iterator< size_t >( f.arity ),
							[&]( size_t i ){
					args.push_back( make_variable( "s" + std::to_string( i ) ) );
					argt.push_back( make_variable( "t" + std::to_string( i ) ) ); } );
				sentence and_stack =
						make_and(
							make_equal( args[0], argt[0] ),
							make_equal( args[1], argt[1] ) );
				try_insert( sequent, make_imply( make_and( and_stack, make_predicate( f.name, args ) ), make_predicate( f.name, argt ) ), true );
			}
		}
		void add_equal_generator( )
		{
			try_insert( sequent, make_all( "t", make_equal( make_variable( "t" ), make_variable( "t" ) ) ), true );
			try_insert( sequent,
						make_all(
							"s1",
							make_all(
								"t1",
								make_all(
									"s2",
									make_all(
										"t2",
										make_imply(
											make_and(
												make_and(
													make_equal(
														make_variable( "s1" ),
														make_variable( "t1" ) ),
													make_equal(
														make_variable( "s2" ),
														make_variable( "t2" ) ) ),
											make_equal(
												make_variable( "s1" ),
												make_variable( "s2" ) ) ),
														make_equal(
															make_variable( "t1" ),
															make_variable( "t2" ) ) ) ) ) ) ),
						true );
			std::for_each( functions.begin( ), functions.end( ), [this]( const function & f ){ add_equal_generator( f ); } );
			std::for_each( predicates.begin( ), predicates.end( ), [this]( const predicate & f ){ add_equal_generator( f ); } );
		}
		explicit operator std::string( ) const
		{
			auto res = pair_str( );
			return res.first + "-->" + res.second;
		}
		std::pair< std::string, std::string > pair_str( ) const
		{
			std::string postive, negative;
			auto function = [&]( const std::pair< sentence, bool > & val )
			{
				std::string & str = val.second ? postive : negative;
				if ( ! str.empty( ) ) { str += ","; }
				str += static_cast< std::string >( val.first );
			};
			std::for_each( temp_sequent.begin( ), temp_sequent.end( ), function );
			std::for_each( sequent.begin( ), sequent.end( ), function );
			std::for_each( expanded.begin( ), expanded.end( ), function );
			return std::make_pair( postive, negative );
		}
		bool is_valid( )
		{
			pt.reset( new proof_tree( static_cast< std::string >( * this ) ) );
			std::shared_ptr< proof_tree > leaf = pt;
			try
			{
				while ( ! sequent.empty( ) || ! temp_sequent.empty( ) )
				{
					if ( sequent.empty( ) )
					{
						sequent.swap( temp_sequent );
						auto f = tg.generate( );
						assert( f.size( ) == 1 );
						sentence_map.insert(
									std::make_pair(
										f[0],
										std::set< sentence >( ) ) );
					}
					while ( ! sequent.empty( ) )
					{
						auto t = * sequent.begin( );
						sequent.erase( sequent.begin( ) );
						//if ( t.first->name == "variable" ) { try_insert( expanded, t.first, t.second ); }
						else if ( t.first.is_quantifier( ) )
						{
							assert( t.first->arguments.size( ) == 2 );
							if ( t.first->cs_type == sentence::type::all )
							{
								if ( t.second )
								{
									std::for_each(
										sentence_map.begin( ),
										sentence_map.end( ),
										[&,this]( auto & s )
										{
											if ( s.second.count( t.first ) == 0 )
											{
												s.second.insert( t.first );
												this->try_insert(
															sequent,
															substitution( { boost::get< variable >( t.first->arguments[0] ), s.first } )
																( t.first->argument[1] ),
															true );
											}
										} );
									try_insert( temp_sequent, t.first, true );
								}
								else { try_insert( sequent, t.first.rebound( boost::get< term >( t.first->arguments[0] ), new_variable( ) ), false ); }
							}
							else
							{
								assert( t.first->cs_type  == sentence::type::some );
								if ( t.second ) { try_insert( sequent, t.first.rebound( boost::get< term >( t.first->arguments[0] ), new_variable( ) ), true ); }
								else
								{
									std::for_each(
												sentence_map.begin( ),
												sentence_map.end( ),
												[&,this]( auto & s )
												{
													if ( s.second.count( t.first ) == 0 )
													{
														s.second.insert( t.first );
														this->try_insert( sequent, t.first.rebound( boost::get< term >( t.first->arguments[0] ), s.first ), false );
													}
												} );
									try_insert( temp_sequent, t.first, false );
								}
							}
						}
						else
						{
							if ( t.first->cs_type == sentence::type::logical_not )
							{
								assert( t.first->arguments.size( ) == 1 );
								try_insert( sequent, boost::get< sentence >( t.first->arguments[0] ), ! t.second );
							}
							else if ( t.first->cs_type == sentence::type::logical_and )
							{
								assert( t.first->arguments.size( ) == 2 );
								if ( t.second )
								{
									try_insert( sequent, boost::get< sentence >( t.first->arguments[0] ), true );
									try_insert( sequent, boost::get< sentence >( t.first->arguments[1] ), true );
								}
								else
								{
									if ( ! is_valid( leaf, boost::get< sentence >( t.first->arguments[0] ), false ) ) { return false; }
									try_insert( sequent, boost::get< sentence >( t.first->arguments[1] ), false );
								}
							}
							else if ( t.first->cs_type == sentence::type::logical_or )
							{
								assert( t.first->arguments.size( ) == 2 );
								if ( t.second )
								{
									if ( ! is_valid( leaf, boost::get< sentence >( t.first->arguments[0] ), true ) ) { return false; }
									try_insert( sequent, boost::get< sentence >( t.first->arguments[1] ), true );
								}
								else
								{
									try_insert( sequent, boost::get< sentence >( t.first->arguments[0] ), false );
									try_insert( sequent, boost::get< sentence >( t.first->arguments[1] ), false );
								}
							}
							//else if ( t.first->cs_type == equal )
							//{
							//	assert( t.first->arguments.size( ) == 2 );
							//	try_insert( expanded, t.first, t.second );
							//	try_insert( expanded, make_equal( t.first->arguments[1], t.first->arguments[0] ), t.second );
							//}
							//else { try_insert( expanded, t.first, t.second ); }
						}
						leaf = join( leaf, std::shared_ptr< proof_tree >( new proof_tree( static_cast< std::string >( * this ), { } ) ) );
					}
				}
				return false;
			}
			catch ( contradiction & con )
			{
				join( leaf, con.pt );
				return true;
			}
		}
		gentzen_system( const gentzen_system & t ) :
			sequent( t.sequent ),
			temp_sequent( t.temp_sequent ),
			cv_map( t.cv_map ),
			sentence_map( t.sentence_map ),
			expanded( t.expanded ),
			unused( t.unused ),
			functions( t.functions ),
			predicates( t.predicates ),
			tg( this, 1, cv_map, functions ) { }
		gentzen_system( const sentence & t ) :
			sequent( { { t, false } } ), functions( t.functions( ) ), predicates( t.predicates( ) ), tg( this, 1, cv_map, functions )
		{
			const auto fv = t.free_variables( );
			const auto con = t.constants( );
			auto r = boost::range::join( fv, con );
			std::transform(
						r.begin( ),
						r.end( ),
						std::inserter( cv_map, cv_map.begin( ) ),
						[]( const term & s ){ return std::make_pair( s, std::set< sentence >( ) ); } );
			sentence_map = cv_map;
			if ( cv_map.empty( ) ) { new_variable( ); }
			if ( t.have_equal( ) ) { add_equal_generator( ); }
		}
		static bool is_valid( sentence & te )
		{
			gentzen_system t( te );
			bool res = t.is_valid( );
			te.pt = t.pt;
			return res;
		}
	};
		*/
}
#endif //FIRST_ORDER_LOGIC_DEDUCTION_TREE