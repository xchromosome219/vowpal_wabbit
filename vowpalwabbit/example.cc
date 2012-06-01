#include <stdint.h>
#include "parse_primitives.h"
#include "v_array.h"
#include "example.h"

example *alloc_example(size_t label_size)
{
  example* ex = (example*)calloc(1, sizeof(example));
  if (ex == NULL) return NULL;
  ex->ld = calloc(1, label_size);
  ex->in_use = true;
  return ex;
}

void dealloc_example(void(*delete_label)(void*), example&ec)
{
  if (delete_label)
    delete_label(ec.ld);
  if (ec.tag.end_array != ec.tag.begin)
    {
      free(ec.tag.begin);
      ec.tag.end_array = ec.tag.begin;
    }
      
  if (ec.topic_predictions.begin)
    free(ec.topic_predictions.begin);

  free(ec.ld);
  for (size_t j = 0; j < 256; j++)
    {
      if (ec.atomics[j].begin != ec.atomics[j].end_array)
        free(ec.atomics[j].begin);

      if (ec.audit_features[j].begin != ec.audit_features[j].end_array)
        {
          for (audit_data* temp = ec.audit_features[j].begin; 
               temp != ec.audit_features[j].end; temp++)
            if (temp->alloced) {
              free(temp->space);
              free(temp->feature);
              temp->alloced = false;
            }
          free(ec.audit_features[j].begin);
        }
    }
  free(ec.indices.begin);
}

feature copy_feature(feature src) {
  feature f = { src.x, src.weight_index };
  return f;
}

void copy_example_data(example &dst, example src, size_t label_size)
{
  if ((label_size == 0) || (!src.ld)) {
    if (dst.ld) free(dst.ld);
    dst.ld = NULL;
  } else {
    memcpy(dst.ld, src.ld, label_size);
  }

  dst.final_prediction = src.final_prediction;

  copy_array(dst.tag, src.tag);
  dst.example_counter = src.example_counter;

  copy_array(dst.indices, src.indices);
  for (size_t i=0; i<256; i++)
    copy_array(dst.atomics[i], src.atomics[i], copy_feature);

  dst.num_features = src.num_features;
  dst.pass = src.pass;
  dst.partial_prediction = src.partial_prediction;
  copy_array(dst.topic_predictions, src.topic_predictions);
  dst.loss = src.loss;
  dst.eta_round = src.eta_round;
  dst.eta_global = src.eta_global;
  dst.global_weight = src.global_weight;
  dst.example_t = src.example_t;
  for (size_t i=0; i<256; i++)
    dst.sum_feat_sq[i] = src.sum_feat_sq[i];
  dst.total_sum_feat_sq = src.total_sum_feat_sq;
  dst.revert_weight = src.revert_weight;
  dst.sorted = src.sorted;
  dst.done = src.done;

  dst.in_use = false;   // is_use is NOT copied!!!
}
