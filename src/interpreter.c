#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
#include "ast.h"
#include "interpreter.h"

void interpret (ScopeStmt* stmt) {
	printf("Interpret program:\n");
	print_scopestmt(stmt);
	printf("\n");
	switch(stmt->tag){
		case VAR_STMT:{
			ScopeVar* s2 = (ScopeVar*)stmt;
			break;
		}
		case FN_STMT:{
			ScopeFn* s2 = (ScopeFn*)stmt;
			break;
		}
		case SEQ_STMT:{
			ScopeSeq* s2 = (ScopeSeq*)stmt;
		break;
		}
		case EXP_STMT:{
			ScopeExp* s2 = (ScopeExp*)stmt;
			break;
		}
		default:
			printf("Unrecognized scope statement with tag %d\n", stmt->tag);
			exit(-1);
	}
}

void exec_stmt(EnvObj* genv, EnvObj* env, ScopeStmt* s){

}
Obj* eval_stmt(EnvObj* genv, EnvObj* env, ScopeStmt* s){

}

Obj* eval_exp(EnvObj* genv, EnvObj* env, Exp* s){
	switch(e->tag){
		case INT_EXP:{
			IntExp* e2 = (IntExp*)e;
			return make_int_obj(e2->value);
		}
		case NULL_EXP:{
			return make_null_obj();
		}
		case PRINTF_EXP:{
			PrintfExp* e2 = (PrintfExp*)e;
			printf("printf(");
			print_string(e2->format);
			for(int i=0; i<e2->nexps; i++){
			  printf(", ");
			  print_exp(e2->exps[i]);
			}
			printf(")");
			break;
		}
		case ARRAY_EXP:{
			ArrayExp* e2 = (ArrayExp*)e;
			return make_array_obj(eval_exp(genv, env, e2->length), eval_exp(genv, env, e2->init));
		}
		case OBJECT_EXP:{
			ObjectExp* e2 = (ObjectExp*)e;
			EnvObj* nenv = make_env_obj(eval_exp(genv, env, e2->parent));
			for(int i=0; i<e2->nslots; i++){
				exec_stmt(genv, nenv, e2->slots[i]);
			}
			return env;
		}
							 // TODO: where I stopped
		case SLOT_EXP:{
			SlotExp* e2 = (SlotExp*)e;
			print_exp(e2->exp);
			printf(".%s", e2->name);
			break;
		}
		case SET_SLOT_EXP:{
			SetSlotExp* e2 = (SetSlotExp*)e;
			print_exp(e2->exp);
			printf(".%s = ", e2->name);
			print_exp(e2->value);
			break;
		}
		case CALL_SLOT_EXP:{
			CallSlotExp* e2 = (CallSlotExp*)e;
			print_exp(e2->exp);
			printf(".%s(", e2->name);
			for(int i=0; i<e2->nargs; i++){
			  if(i > 0) printf(", ");
			  print_exp(e2->args[i]);
			}
			printf(")");
			break;
		}
		case CALL_EXP:{
			CallExp* e2 = (CallExp*)e;
			printf("%s(", e2->name);
			for(int i=0; i<e2->nargs; i++){
			  if(i > 0) printf(", ");
			  print_exp(e2->args[i]);
			}
			printf(")");
			break;
		}
		case SET_EXP:{
			SetExp* e2 = (SetExp*)e;
			printf("%s = ", e2->name);
			print_exp(e2->exp);
			break;
		}
		case IF_EXP:{
			IfExp* e2 = (IfExp*)e;
			printf("if ");
			print_exp(e2->pred);
			printf(" : (");
			print_scopestmt(e2->conseq);
			printf(") else : (");
			print_scopestmt(e2->alt);
			printf(")");
			break;
		}
		case WHILE_EXP:{
			WhileExp* e2 = (WhileExp*)e;
			printf("while ");
			print_exp(e2->pred);
			printf(" : (");
			print_scopestmt(e2->body);
			printf(")");
			break;
		}
		case REF_EXP:{
			RefExp* e2 = (RefExp*)e;
			printf("%s", e2->name);
			break;
		}
		default:
			printf("Unrecognized Expression with tag %d\n", e->tag);
			exit(-1);
	}
}
