/**
 * RuleShape.cpp
 *
 * A RuleShape contains a Configuration that describes how a rule could
 * pull or push charge. It is not directly associated with a specific rule, 
 * but instead is a template for a rule.
 * 
 * When generating constraints, this rule determines which variable should be used
 * specifically between two chargeable objects, depending on the situation of the 
 * given Configuration.
 */


#include "Grid.hpp"
#include "Configuration.hpp"
#include "Rule.hpp"
#include "macros.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "RuleShape.hpp"


using namespace adage::grids;



RuleShape::RuleShape()
{
	this->shape_conf = 0;

	this->center_is_puller = false;

	this->rule_name = 0;
	this->var_name = 0;

	this->rule_type = -1;

	this->center_id = -1;
	this->center_is_face = false;

	this->size_chargeable = 12;
	this->num_chargeable= 0;
	this->chargeable = (int*)malloc(this->size_chargeable * sizeof(int));
		
	this->num_kernel_keys = 0;
	this->num_kernel_vertices = 0;
	this->kernel_vertex_indices = 0;
	this->kernel_vertex_coefficients = 0;
}

RuleShape::RuleShape(RuleShape* shape, int* fperm, int* vperm, int constraint_center, bool center_is_puller)
{
	this->center_is_puller = center_is_puller;
	this->center_is_which_chargeable = -1;

	this->rule_name = (char*)malloc(strlen(shape->rule_name) + 1);
	this->var_name = (char*)malloc(strlen(shape->var_name) + 1);

	strcpy(this->rule_name, shape->rule_name);
	strcpy(this->var_name, shape->var_name);

	this->shape_conf = shape->shape_conf->duplicate(fperm, vperm);
	this->rule_type = shape->rule_type;
	
	this->center_is_face = shape->center_is_face;

	if ( shape->center_is_face )
	{
		this->center_id = fperm[shape->center_id];
	}
	else	
	{
		this->center_id = vperm[shape->center_id];
	}


	this->size_chargeable = shape->size_chargeable;
	this->num_chargeable = shape->num_chargeable;
	this->chargeable = (int*)malloc(this->size_chargeable * sizeof(int));

	int* charge_perm = 0;

	if ( this->rule_type == VARIABLE_TYPE_F2V || this->rule_type == VARIABLE_TYPE_F2F )
	{
		charge_perm = fperm;
	}
	else
	{
		charge_perm = vperm;
	}

	for ( int i = 0; i < shape->num_chargeable; i++ )
	{
		this->chargeable[i] = charge_perm[shape->chargeable[i]];

		if ( !center_is_puller && this->chargeable[i] == constraint_center )
		{
			this->center_is_which_chargeable = i;
		}
	}

	this->num_kernel_keys = shape->num_kernel_keys;
	this->num_kernel_vertices = (int*)malloc(this->num_kernel_keys * sizeof(int));
	this->kernel_vertex_indices = (int**)malloc(this->num_kernel_keys * sizeof(int*));
	this->kernel_vertex_coefficients = (int**)malloc(this->num_kernel_keys * sizeof(int*));

	for ( int i = 0; i < this->num_kernel_keys; i++ )
	{
		this->num_kernel_vertices[i] = shape->num_kernel_vertices[i];

		this->kernel_vertex_indices[i] = (int*)malloc(shape->num_kernel_vertices[i]*sizeof(int));
		this->kernel_vertex_coefficients[i] = (int*)malloc(shape->num_kernel_vertices[i]*sizeof(int));

		for ( int j =0 ; j < this->num_kernel_vertices[i]; j++ )
		{
			this->kernel_vertex_coefficients[i][j] = shape->kernel_vertex_coefficients[i][j];
			this->kernel_vertex_indices[i][j] = vperm[shape->kernel_vertex_indices[i][j]];
		}
	}
}

RuleShape::~RuleShape()
{
	delete this->shape_conf;
	this->shape_conf = 0;

	FREE_ARRAY2(this->kernel_vertex_coefficients, this->num_kernel_keys);
	FREE_ARRAY2(this->kernel_vertex_indices, this->num_kernel_keys);
	FREE_ARRAY(this->num_kernel_vertices);

	FREE_ARRAY(this->chargeable);

	FREE_ARRAY(this->rule_name);
	FREE_ARRAY(this->var_name);
}

/**
 * modifyCoefficients modifies a list of coefficients depending on the given configuration.
 */
void RuleShape::modifyCoefficients(Configuration* conf, LinearConstraint* constraint)
{
	int num_keys = this->num_kernel_keys;

	if ( this->num_chargeable > 1 )
	{
		num_keys++;
	}

	int* keys = (int*)malloc(num_keys * sizeof(int));

	for ( int i = 0;i < this->num_kernel_keys; i++ )
	{
		keys[i] = 0;

		for ( int j = 0; j < this->num_kernel_vertices[i]; j++ )
		{
			if ( conf->isElement(this->kernel_vertex_indices[i][j]) )
			{
				keys[i] += this->kernel_vertex_coefficients[i][j];
			}
		}
	}

	int coef = 0;

	if ( this->center_is_which_chargeable >= 0 )
	{
		if ( this->num_chargeable > 1 )
		{
			keys[this->num_kernel_keys] = this->center_is_which_chargeable;
		}

		coef = -1;

		constraint->insertMonomial(coef, this->var_name, num_keys, keys);
	}
	else
	{
		// add all of them!
		for ( int i = 0;i < this->num_chargeable; i++ )
		{
			if ( this->num_chargeable > 1 )
			{
				keys[this->num_kernel_keys] = i;
			}
			coef = 1;

			constraint->insertMonomial(coef, this->var_name, num_keys, keys);
		}	
	}

	free(keys);
}



void RuleShape::print()
{
	this->write(stdout);
}

void RuleShape::write(FILE* file)
{
	fprintf(file, "#begin RuleShape\n");

	fprintf(file, "rule_name = \"%s\";\n", this->rule_name);

	fprintf(file, "var_name = \"%s\";\n", this->var_name);

	fprintf(file, "rule_type = %d;\n", this->rule_type);

	fprintf(file, "keys = [ \\\n");

	for ( int i = 0; i < this->num_kernel_keys; i++ )
	{
		fprintf(file, "    ");

		for ( int j = 0; j < this->num_kernel_vertices[i]; j++ )
		{
			fprintf(file, "%d * x[%d]", this->kernel_vertex_coefficients[i][j], this->kernel_vertex_indices[i][j]);

			if ( j < this->num_kernel_vertices[i] - 1)
			{
				fprintf(file, " + ");
			}
		}

		fprintf(file, ", \\\n");
	}

	fprintf(file, "  ];\n");

	if ( this->center_id >= 0 )
	{
		fprintf(file, "center_id = %d;\ncenter_is_face = ", this->center_id);

		if ( this->center_is_face )
		{
			fprintf(file, "true;\n");
		} else {
			fprintf(file, "false;\n");
		}
	}

	fprintf(file, "chargeable = [ ");
	for ( int i = 0; i < this->num_chargeable; i++ )
	{
		fprintf(file, "%d", this->chargeable[i]);

		if ( i < this->num_chargeable - 1 )
		{
			fprintf(file, ", ");
		}
	}
	fprintf(file, " ];\n");

	this->shape_conf->write(file);
	fprintf(file, "#end RuleShape\n");
}

RuleShape* RuleShape::read(Grid* grid, FILE* file)
{
	size_t buflen = 100;
	char* buffer = (char*)malloc(buflen);

	RuleShape* shape = new RuleShape();

	while ( getline(&buffer, &buflen, file) > 0 )
	{
		if ( strstr(buffer, "rule_name = ") != 0 )
		{
			shape->rule_name = (char*)malloc(strlen(buffer) - strlen("rule_name = \"") + 1);
			strcpy(shape->rule_name, buffer + strlen("rule_name = \""));
			char* endquote = strstr(shape->rule_name, "\"");
			*endquote = 0;
		}
		if ( strstr(buffer, "var_name = ") != 0 )
		{
			shape->var_name = (char*)malloc(strlen(buffer) - strlen("var_name = \"") + 1);
			strcpy(shape->var_name, buffer + strlen("var_name = \""));
			char* endquote = strstr(shape->var_name, "\"");
			*endquote = 0;
		}
		if ( strstr(buffer, "rule_type = ") != 0 )
		{
			shape->rule_type = atoi(buffer + strlen("rule_type = "));
		}
		else if ( strstr(buffer, "center_id = ") != 0 )
		{
			shape->center_id = atoi(buffer + strlen("center_id = "));
		}
		else if ( strstr(buffer, "chargeable = [") != 0 )
		{
			char* charge = buffer;
			char* next = strtok(charge + strlen("chargeable = ["), " ,");

			while ( next != 0 )
			{
				if ( next[0] == ']' )
				{
					break;
				}

				int val = atoi(next);

				shape->addChargeable(val);

				next = strtok(0, " ,");
			}
			
			free(charge);

			buffer = 0;
			buflen = 0;
		}
		else if ( strstr(buffer, "center_is_face = ") != 0 )
		{
			if ( strcmp(buffer + strlen("center_is_face = "), "true;\n") == 0 )
			{
				shape->center_is_face = true;
			}
			else
			{
				shape->center_is_face = false;
			}
		}
		else if ( strcmp(buffer, "#begin Configuration\n") == 0 )
		{
			shape->shape_conf = Configuration::read(grid, file);
		}
		else if ( strcmp(buffer, "keys = [ \\\n") == 0 )
		{
			int size_keys = 20;
			shape->num_kernel_keys = 0;

			shape->num_kernel_vertices = (int*)malloc(size_keys * sizeof(int));
			shape->kernel_vertex_coefficients = (int**)malloc(size_keys * sizeof(int*));
			shape->kernel_vertex_indices = (int**)malloc(size_keys * sizeof(int*));

			while ( getline(&buffer, &buflen, file) > 0  && strcmp(buffer, "  ];\n") != 0 )
			{

				// check for a key!
				char* charge = buffer;

				shape->num_kernel_vertices[shape->num_kernel_keys] = 0;
				shape->kernel_vertex_indices[shape->num_kernel_keys] = (int*)malloc(20*sizeof(int));
				shape->kernel_vertex_coefficients[shape->num_kernel_keys] = (int*)malloc(20*sizeof(int));

				int cur_num_kv = 0;

				char* next = strtok(charge, "+");

				while ( next != 0 )
				{
					int coef = atoi(next);
					int var = atoi(strstr(next, "x[") + 2);

					shape->kernel_vertex_coefficients[shape->num_kernel_keys][cur_num_kv] = coef;
					shape->kernel_vertex_indices[shape->num_kernel_keys][cur_num_kv] = var;

					cur_num_kv++;

					next = strtok(0, "+");
				}

				shape->num_kernel_vertices[shape->num_kernel_keys] = cur_num_kv;
				(shape->num_kernel_keys)++;
			}
		}
		else if ( strcmp(buffer, "#end RuleShape\n") == 0 )
		{
			break;
		}
	}

	free(buffer);

	return shape;
}

int RuleShape::getType()
{
	return this->rule_type;
}

int RuleShape::getCenter()
{
	return this->center_id;
}

bool RuleShape::isCenterFace()
{
	return this->center_is_face;
}

Configuration* RuleShape::getShape()
{
	return this->shape_conf;
}

char* RuleShape::getRuleName()
{
	return this->rule_name;
}


int RuleShape::getNumChargeable()
{
	return this->num_chargeable;
}

int RuleShape::getChargeable(int i)
{
	return this->chargeable[i];
}

void RuleShape::addChargeable(int i)
{
	if ( this->num_chargeable >= this->size_chargeable )
	{
		this->size_chargeable += 10;
		this->chargeable = (int*)realloc(this->chargeable, this->size_chargeable * sizeof(int));
	}

	for ( int j = 0; j < this->num_chargeable; j++ )
	{
		if ( this->chargeable[j] == i )
		{
			return;
		}
	}

	this->chargeable[(this->num_chargeable)++] = i;
}



