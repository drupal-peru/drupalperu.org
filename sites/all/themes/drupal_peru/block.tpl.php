<?php


$edit_links = l(t('edit block'), 'admin/build/block/configure/'. $block->module .'/'. $block->delta, array('title' => t('edit the content of this block'), 'class' => 'block-edit'), drupal_get_destination(), NULL, FALSE, TRUE);
?>
<div id="block-<?php print $block->module .'-'. $block->delta; ?>" class="block <?php print $block_classes; ?>">

  <?php if ($block->subject): ?>
    <h2><?php print $block->subject ?></h2>
  <?php endif;?>

  <div class="content">
    <?php print $block->content ?>
  </div>

<?php if (user_access('administer blocks')) :?>  
  <div class="edit"><?php print $edit_links; ?></div>
<?php endif; ?>

</div>
