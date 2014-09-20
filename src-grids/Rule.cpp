/*
 * Rule.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: stolee
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.hpp"
#include "Configuration.hpp"
#include "Rule.hpp"
#include "RuleShape.hpp"

using namespace adage::grids;

Rule::Rule()
{
	this->conf = 0;
	this->shape = 0;
	this->values_are_set = 0;
	this->variable_index = 0;
	this->variable_from_color = 0;
	this->variable_to_color = 0;
	this->variable_from_mult = 0;
	this->variable_to_mult = 0;
	this->variable_type = 0;
	this->variable_value = 0;
	this->num_variables = 0;
}

/**
 * Create a rule from a configuration.
 *
 * Will use "duplicate" to make a deep copy.
 *
 * The first constructor goes from all colors to the center color.
 * The second constructor allows the user to specify which colors can actually send charge.
 */
Rule::Rule(const Configuration* conf, int rule_type, int base_index)
{
	this->conf = conf->duplicate();
	this->rule_type = rule_type;
	this->shape = 0;

	this->conf->computeSymmetry(); // Do not allow flip (yet)!

	int num_v_colors = this->conf->getNumVertexColors();
	int num_f_colors = this->conf->getNumFaceColors();

	this->num_variables = 0;
	switch( rule_type )
	{
		case VARIABLE_TYPE_V2V:
			this->num_variables = num_v_colors * (num_v_colors);
		break;

		case VARIABLE_TYPE_V2F:
			this->num_variables = num_v_colors * num_f_colors;
		break;

		case VARIABLE_TYPE_F2V:
			this->num_variables = num_f_colors * num_v_colors;
		break;

		case VARIABLE_TYPE_F2F:
			this->num_variables = num_f_colors * (num_f_colors);
		break;

	}

	this->values_are_set = false;

	this->variable_index = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_from_color = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_to_color = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_from_mult = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_to_mult = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_type = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_value = (fraction*)malloc(this->num_variables*sizeof(fraction));

	int cur_rule = 0;
	switch( rule_type )
	{	case VARIABLE_TYPE_V2V:
			for ( int i = 0; i < num_v_colors; i++ )
			{
				for ( int j = 0; j < num_v_colors; j++ )
				{
					this->variable_index[cur_rule] = base_index++;
					this->variable_from_color[cur_rule] = i;
					this->variable_to_color[cur_rule] = j;
					this->variable_type[cur_rule] =- VARIABLE_TYPE_V2V;
					this->variable_from_mult[cur_rule] = this->conf->getVertexColorClassSize(i);
					this->variable_to_mult[cur_rule] = this->conf->getVertexColorClassSize(j);
					cur_rule++;
				}
			}
		break;
		case VARIABLE_TYPE_V2F:
			for ( int i = 0; i < num_v_colors; i++ )
			{
				for ( int j = 0; j < num_f_colors; j++ )
				{
					this->variable_index[cur_rule] = base_index++;
					this->variable_from_color[cur_rule] = i;
					this->variable_to_color[cur_rule] = j;
					this->variable_type[cur_rule] =- VARIABLE_TYPE_V2F;
					this->variable_from_mult[cur_rule] = this->conf->getVertexColorClassSize(i);
					this->variable_to_mult[cur_rule] = this->conf->getFaceColorClassSize(j);
					cur_rule++;
				}
			}
		break;
		case VARIABLE_TYPE_F2V:
			for ( int i = 0; i < num_f_colors; i++ )
			{
				for ( int j = 0; j < num_v_colors; j++ )
				{
					this->variable_index[cur_rule] = base_index++;
					this->variable_from_color[cur_rule] = i;
					this->variable_to_color[cur_rule] = j;
					this->variable_type[cur_rule] =- VARIABLE_TYPE_F2V;
					this->variable_from_mult[cur_rule] = this->conf->getFaceColorClassSize(i);
					this->variable_to_mult[cur_rule] = this->conf->getVertexColorClassSize(j);
					cur_rule++;
				}
			}
		break;
		case VARIABLE_TYPE_F2F:
			for ( int i = 0; i < num_f_colors; i++ )
			{
				for ( int j = 0; j < num_f_colors; j++ )
				{
					this->variable_index[cur_rule] = base_index++;
					this->variable_from_color[cur_rule] = i;
					this->variable_to_color[cur_rule] = j;
					this->variable_type[cur_rule] =- VARIABLE_TYPE_F2F;
					this->variable_from_mult[cur_rule] = this->conf->getFaceColorClassSize(i);
					this->variable_to_mult[cur_rule] = this->conf->getFaceColorClassSize(j);
					cur_rule++;
				}
			}
		break;
	}
}

Rule::Rule(const Configuration* conf, int rule_type, int to_color, int* from_colors, int num_from, int base_index)
{
	this->conf = conf->duplicate();
	this->shape = 0;

	this->rule_type = rule_type;

	this->conf->computeSymmetry(); // Do not allow flip (yet!)

	this->num_variables = num_from;

	this->variable_index = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_from_color = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_to_color = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_from_mult = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_to_mult = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_type = (int*)malloc(this->num_variables*sizeof(int));
	this->variable_value = (fraction*)malloc(this->num_variables*sizeof(fraction));

	this->values_are_set = false;

	int cur_rule = 0;

	for ( int i = 0; i < num_from; i++ )
	{
		this->variable_index[cur_rule] = base_index++;

		this->variable_from_color[cur_rule] = from_colors[i];
		this->variable_to_color[cur_rule] = to_color;
		this->variable_type[cur_rule] = rule_type;

		switch ( rule_type )
		{
			case VARIABLE_TYPE_F2F:
			case VARIABLE_TYPE_F2V:
				this->variable_from_mult[cur_rule] = this->conf->getFaceColorClassSize(from_colors[i]);
				break;

			case VARIABLE_TYPE_V2F:
			case VARIABLE_TYPE_V2V:
				this->variable_from_mult[cur_rule] = this->conf->getVertexColorClassSize(from_colors[i]);
				break;
		}
		
		switch ( rule_type )
		{
			case VARIABLE_TYPE_F2F:
			case VARIABLE_TYPE_V2F:
				this->variable_to_mult[cur_rule] = this->conf->getFaceColorClassSize(to_color);
				break;

			case VARIABLE_TYPE_F2V:
			case VARIABLE_TYPE_V2V:
				this->variable_to_mult[cur_rule] = this->conf->getVertexColorClassSize(to_color);
				break;
		}

		cur_rule++;
	}

}

Rule::~Rule()
{
	delete this->conf;
	if (this->shape != 0 )
	{
		delete this->shape;
		this->shape = 0;
	}
	FREE_ARRAY(this->variable_index);
	FREE_ARRAY(this->variable_from_color);
	FREE_ARRAY(this->variable_to_color);
	FREE_ARRAY(this->variable_from_mult);
	FREE_ARRAY(this->variable_to_mult);
	FREE_ARRAY(this->variable_type);
	FREE_ARRAY(this->variable_value);
}

int Rule::getNumVariables()
{
	return this->num_variables;
}

int Rule::getVariableIndex(int i)
{
	return this->variable_index[i];
}

int Rule::getType()
{
	return this->rule_type;
}


void Rule::setShape(RuleShape* shape)
{
	this->shape = shape->duplicate();
}

void Rule::setShapeConfiguration(Configuration* conf)
{
	this->shape = new RuleShape(conf, this->rule_type);
}

RuleShape* Rule::getShape()
{
	return this->shape;
}


/**
 * Get Access to the configuration, with symmetry calculated.
 */
Configuration*
Rule::getConfiguration()
{
	return this->conf;
}

/**
 * Get everything about a single variable, and quickly.
 */
int Rule::getVariableInfo(int i, int& v_index, int& type, int& from_color, int& to_color, int& from_mult, int& to_mult, fraction& value)
{
	v_index = this->variable_index[i];
	type = this->variable_type[i];
	from_color = this->variable_from_color[i];
	to_color = this->variable_to_color[i];
	from_mult = this->variable_from_mult[i];
	to_mult = this->variable_to_mult[i];
	value = this->variable_value[i];
	
	return 0;
}

/**
 * For FileIO
 */
void Rule::print() const
{
	this->write(stdout);
}

void Rule::write(FILE* file) const
{
	fprintf(file, "#begin Rule\n");

	this->conf->write(file);

	if ( this->shape != 0 )
	{
		this->shape->write(file);
	}


	fprintf(file, "num_variables = %3d;\n", this->num_variables);

	fprintf(file, "variable_index = [ ");
	for ( int i = 0; i < this->num_variables; i++ )
	{
		fprintf(file, "%2d", this->variable_index[i]);

		if ( i < this->num_variables-1 )
		{
			fprintf(file, ", ");
		}
	}
	fprintf(file, " ];\n");

	fprintf(file, "variable_from_color = [ ");
	for ( int i = 0; i < this->num_variables; i++ )
	{
		fprintf(file, "%2d", this->variable_from_color[i]);

		if ( i < this->num_variables-1 )
		{
			fprintf(file, ", ");
		}
	}
	fprintf(file, " ];\n");



	fprintf(file, "variable_to_color = [ ");
	for ( int i = 0; i < this->num_variables; i++ )
	{
		fprintf(file, "%2d", this->variable_to_color[i]);

		if ( i < this->num_variables-1 )
		{
			fprintf(file, ", ");
		}
	}
	fprintf(file, " ];\n");



	fprintf(file, "variable_from_mult = [ ");
	for ( int i = 0; i < this->num_variables; i++ )
	{
		fprintf(file, "%2d", this->variable_from_mult[i]);

		if ( i < this->num_variables-1 )
		{
			fprintf(file, ", ");
		}
	}
	fprintf(file, " ];\n");



	fprintf(file, "variable_to_mult = [ ");
	for ( int i = 0; i < this->num_variables; i++ )
	{
		fprintf(file, "%2d", this->variable_to_mult[i]);

		if ( i < this->num_variables-1 )
		{
			fprintf(file, ", ");
		}
	}
	fprintf(file, " ];\n");



	fprintf(file, "variable_type = [ ");
	for ( int i = 0; i < this->num_variables; i++ )
	{
		fprintf(file, "%2d", this->variable_type[i]);

		if ( i < this->num_variables-1 )
		{
			fprintf(file, ", ");
		}
	}
	fprintf(file, " ];\n");

	if( this->values_are_set )
	{
		fprintf(file, "variable_value = [ ");
		for ( int i = 0; i < this->num_variables; i++ )
		{
			fprintf(file, "%4lld / %4llu", this->variable_value[i].a,  this->variable_value[i].b);

			if ( i < this->num_variables-1 )
			{
				fprintf(file, ", ");
			}
		}
		fprintf(file, " ];\n");
	}

	fprintf(file, "#end Rule\n");
}

Rule* Rule::read(Grid* grid, FILE* file)
{	
	size_t line_len = 1000;
	char* line = (char*)malloc(line_len);

	Configuration* conf = 0;
	Rule* rule = new Rule();

	while ( getline( &line, &line_len, file) > 0 )
	{
		if ( strcmp(line, "#begin Rule\n") == 0 )
		{
		}

		if ( strcmp(line, "#begin Configuration\n") == 0 )
		{
			conf = Configuration::read(grid, file);
			rule->conf = conf;
			continue;
		}

		if ( strcmp(line, "#begin RuleShape\n") == 0 )
		{
			rule->shape = RuleShape::read(grid, file);
			continue;
		}

		if ( strcmp(line, "#end Rule\n") == 0 )
		{
			break;
		}

		if ( strstr(line, "num_variables = ") != 0 )
		{
			rule->num_variables = atoi(line + strlen("num_variables = "));

			rule->variable_index = (int*)malloc(rule->num_variables*sizeof(int));
			rule->variable_from_color = (int*)malloc(rule->num_variables*sizeof(int));
			rule->variable_to_color = (int*)malloc(rule->num_variables*sizeof(int));
			rule->variable_from_mult = (int*)malloc(rule->num_variables*sizeof(int));
			rule->variable_to_mult = (int*)malloc(rule->num_variables*sizeof(int));
			rule->variable_type = (int*)malloc(rule->num_variables*sizeof(int));
			rule->variable_value = (fraction*)malloc(rule->num_variables*sizeof(fraction));
			continue;
		}

		if ( strstr(line, "variable_index = [") != 0 )
		{
			int i = 0;
			char* next = strtok(line + strlen("variable_index = ["), " ,");

			while ( next != 0 )
			{
				if ( next[0] == ']' )
				{
					break;
				}

				int val = atoi(next);

				rule->variable_index[i] = val;

				i++;
				next = strtok(0, " ,");
			}
		}

		if ( strstr(line, "variable_from_color = [") != 0 )
		{
			int i = 0;
			char* next = strtok(line + strlen("variable_from_color = ["), " ,");

			while ( next != 0 )
			{
				if ( next[0] == ']' )
				{
					break;
				}

				int val = atoi(next);

				rule->variable_from_color[i] = val;

				i++;
				next = strtok(0, " ,");
			}
		}



		if ( strstr(line, "variable_to_color = [") != 0 )
		{
			int i = 0;
			char* next = strtok(line + strlen("variable_to_color = ["), " ,");

			while ( next != 0 )
			{
				if ( next[0] == ']' )
				{
					break;
				}

				int val = atoi(next);

				rule->variable_to_color[i] = val;

				i++;
				next = strtok(0, " ,");
			}
			continue;
		}



		if ( strstr(line, "variable_from_mult = [") != 0 )
		{
			int i = 0;
			char* next = strtok(line + strlen("variable_from_mult = ["), " ,");

			while ( next != 0 )
			{
				if ( next[0] == ']' )
				{
					break;
				}

				int val = atoi(next);

				rule->variable_from_mult[i] = val;

				i++;
				next = strtok(0, " ,");
			}
			continue;
		}



		if ( strstr(line, "variable_to_mult = [") != 0 )
		{
			int i = 0;
			char* next = strtok(line + strlen("variable_to_mult = ["), " ,");

			while ( next != 0 )
			{
				if ( next[0] == ']' )
				{
					break;
				}

				int val = atoi(next);

				rule->variable_to_mult[i] = val;

				i++;
				next = strtok(0, " ,");
			}
			continue;
		}


		if ( strstr(line, "variable_type = [") != 0 )
		{
			int i = 0;
			char* next = strtok(line + strlen("variable_type = ["), " ,");

			while ( next != 0 )
			{
				if ( next[0] == ']' )
				{
					break;
				}

				int val = atoi(next);

				rule->variable_type[i] = val;
				rule->rule_type = val;

				i++;
				next = strtok(0, " ,");
			}

			continue;
		}



		if ( strstr(line, "variable_value = [") != 0 )
		{
			int i = 1;
			char* next = strtok(line + strlen("variable_value = ["), ",");

			while ( next != 0 )
			{
				if ( next[0] == ']' )
				{
					break;
				}

				int vala = atoi(next);
				char* bstr = strchr(next, '/') + 1;
				int valb = atoi(bstr);

				rule->variable_value[i].a = vala;
				rule->variable_value[i].b = valb;

				i++;
				next = strtok(0, " ,");
			}

			rule->values_are_set = true;
			continue;
		}
	}

	free(line);

	return rule;
} // Returns 0 if EOF.

